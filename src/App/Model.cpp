#include "Model.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Model::Model()
	: model_(nullptr)
{
	initializeOpenGLFunctions();

	program_ = std::make_unique<QOpenGLShaderProgram>();
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/gbuffer.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/gbuffer.fs");
	program_->link();

	mModel_.setToIdentity();
	mModel_.scale(0.01f, 0.01f, 0.01f);
}

Model::~Model()
{
	if (model_)
	{
		delete model_;
	}
}

bool Model::load(const QString & path)
{
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	model_ = new tinygltf::Model();
	bool res = loader.LoadBinaryFromFile(model_, &err, &warn, path.toStdString());

	if (!err.empty())
	{

		qDebug() << "Model::Load(" << path << ") ERROR: " << QString::fromStdString(err);
	}

	if (!warn.empty())
	{
		qDebug() << "Model::Load(" << path << ") WARNING: " << QString::fromStdString(warn);
	}

	if (!res)
	{
		delete model_;
		return false;
	}

	return true;
}


void Model::bind()
{
	assert(loaded());

	vbos_.clear();

	vao_.create();
	vao_.bind();

	const tinygltf::Scene & scene = model_->scenes[model_->defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model_->nodes.size()));
		bindModelNodes(model_->nodes[scene.nodes[i]]);
	}

	vao_.release();

	// cleanup vbos but do not delete index buffers yet
	for (auto it = vbos_.begin(); it != vbos_.end();)
	{
		tinygltf::BufferView bufferView = model_->bufferViews[it.key()];
		if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER)
		{
			vbos_[it.key()].destroy();
			vbos_.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void Model::draw()
{
	vao_.bind();

	program_->setUniformValue("model", mModel_);

	if (texture_)
	{
		texture_->bind(GL_TEXTURE0);
	}

	const tinygltf::Scene & scene = model_->scenes[model_->defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		drawModelNodes(model_->nodes[scene.nodes[i]]);
	}

	if (texture_) 
	{
		texture_->release(GL_TEXTURE0);
	}
	vao_.release();
	program_->release();
}

void Model::setScale(const float scale)
{
	mModel_.setToIdentity();
	mModel_.scale(scale, scale, scale);
}

void Model::bindProgram()
{
	if (program_)
	{
		program_->bind();
	}
}

void Model::setUniformValue(const QString & uniformName, const float value)
{
	if (program_)
	{
		program_->setUniformValue(uniformName.toStdString().c_str(), value);
	}
}

void Model::setUniformValue(const QString & uniformName, const QVector3D & value)
{
	if (program_)
	{
		program_->setUniformValue(uniformName.toStdString().c_str(), value);
	}
}

void Model::setUniformValue(const QString & uniformName, const QMatrix4x4 & value)
{
	if (program_)
	{
		program_->setUniformValue(uniformName.toStdString().c_str(), value);
	}
}

void Model::bindModelNodes(const tinygltf::Node & node)
{
	if ((node.mesh >= 0) && (node.mesh < model_->meshes.size()))
	{
		bindMesh(model_->meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		assert((node.children[i] >= 0) && (node.children[i] < model_->nodes.size()));
		bindModelNodes(model_->nodes[node.children[i]]);
	}
}

void Model::bindMesh(const tinygltf::Mesh & mesh)
{
	for (size_t i = 0; i < model_->bufferViews.size(); ++i)
	{
		const tinygltf::BufferView & bufferView = model_->bufferViews[i];
		if (bufferView.target == 0)
		{// TODO impl drawarrays
			qDebug() << "WARN: bufferView.target is zero";
			continue;
		}

		const tinygltf::Buffer & buffer = model_->buffers[bufferView.buffer];
		qDebug() << "bufferview.target " << bufferView.target;

		QOpenGLBuffer::Type bufferType = QOpenGLBuffer::Type::VertexBuffer;
		switch (bufferView.target)
		{
			case GL_ARRAY_BUFFER: {
				bufferType = QOpenGLBuffer::Type::VertexBuffer;
				break;
			}
			case GL_ELEMENT_ARRAY_BUFFER: {
				bufferType = QOpenGLBuffer::Type::IndexBuffer;
				break;
			}
			default: {
				qDebug() << "Unsupported buffer target: " << bufferView.target;
				continue;
			}
		}

		vbos_[static_cast<int>(i)] = std::move(QOpenGLBuffer(bufferType));
		QOpenGLBuffer & vbo = vbos_[static_cast<int>(i)];
		vbo.create();
		vbo.bind();
		vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

		qDebug() << "buffer.data.size = " << buffer.data.size()
				 << ", bufferview.byteOffset = " << bufferView.byteOffset;

		vbo.allocate(&buffer.data.at(0) + bufferView.byteOffset, static_cast<int>(bufferView.byteLength));
	}

	program_->bind();

	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model_->accessors[primitive.indices];

		for (auto & attrib: primitive.attributes)
		{
			tinygltf::Accessor accessor = model_->accessors[attrib.second];
			int byteStride =
				accessor.ByteStride(model_->bufferViews[accessor.bufferView]);

			QOpenGLBuffer & vbo = vbos_[accessor.bufferView];
			vbo.bind();

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR)
			{
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0)
				vaa = 0;
			if (attrib.first.compare("NORMAL") == 0)
				vaa = 1;
			if (attrib.first.compare("TEXCOORD_0") == 0)
				vaa = 2;
			if (vaa > -1)
			{
				program_->enableAttributeArray(vaa);
				program_->setAttributeBuffer(vaa, static_cast<GLenum>(accessor.componentType), static_cast<int>(accessor.byteOffset), size, byteStride);
				/*glVertexAttribPointer(vaa, size, accessor.componentType,
									  accessor.normalized ? GL_TRUE : GL_FALSE,
									  byteStride, BUFFER_OFFSET(accessor.byteOffset));*/
			}
			else
			{
				qDebug() << "vaa missing: " << QString::fromStdString(attrib.first);
			}
		}

		if (model_->textures.size() > 0)
		{
			tinygltf::Texture & tex = model_->textures[0];

			if (tex.source > -1)
			{
				bool error = false;
			
				tinygltf::Image & image = model_->images[tex.source];
				
				QImage::Format imageFormat = QImage::Format::NImageFormats;
				
				if (image.bits == 8 && image.component == 3)
				{
					imageFormat = QImage::Format::Format_RGB888;
				}
				else if (image.bits == 8 && image.component == 4)
				{
					imageFormat = QImage::Format::Format_RGBA8888;
				}
				else if (image.bits == 16 && image.component == 3)
				{
					imageFormat = QImage::Format::Format_RGB32;
				}
				else if (image.bits == 16 && image.component == 4)
				{
					imageFormat = QImage::Format::Format_RGBA64;
				}
				else
				{
					qDebug() << "Unsupported texture format: image.bits=" << image.bits << "\timage.component=" << image.component;
					error = true;
				}

				if (!error)
				{
					texture_ = std::make_unique<QOpenGLTexture>(QImage(image.image.data(), image.width, image.height, imageFormat), QOpenGLTexture::MipMapGeneration::DontGenerateMipMaps);
					texture_->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
					texture_->setWrapMode(QOpenGLTexture::Repeat);
				}
				else {
					qDebug() << "Couldn't create texture :(";
				}
			}
		}
	}
}

void Model::drawModelNodes(const tinygltf::Node & node)
{
	if ((node.mesh >= 0) && (node.mesh < model_->meshes.size()))
	{
		drawMesh(model_->meshes[node.mesh]);
	}
	for (size_t i = 0; i < node.children.size(); i++)
	{
		drawModelNodes(model_->nodes[node.children[i]]);
	}
}

void Model::drawMesh(const tinygltf::Mesh & mesh)
{
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model_->accessors[primitive.indices];

		QOpenGLBuffer & vbo = vbos_[indexAccessor.bufferView];
		vbo.bind();
	
		glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count),
					   indexAccessor.componentType,
					   BUFFER_OFFSET(indexAccessor.byteOffset));
	}
}
