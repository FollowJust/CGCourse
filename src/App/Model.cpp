#include "Model.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <string>

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
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/diffuse.fs");
	program_->link();
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

void Model::draw(const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection)
{
	vao_.bind();

	const tinygltf::Scene & scene = model_->scenes[model_->defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		drawModelNodes(model_->nodes[scene.nodes[i]], mModel, mView, mProjection);
	}

	vao_.release();
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

		vbos_[i] = std::move(QOpenGLBuffer(bufferType));
		QOpenGLBuffer & vbo = vbos_[i];
		vbo.create();
		vbo.bind();
		vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

		qDebug() << "buffer.data.size = " << buffer.data.size()
				 << ", bufferview.byteOffset = " << bufferView.byteOffset;

		vbo.allocate(&buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
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

		//if (model.textures.size() > 0)
		//{
		//	// fixme: Use material's baseColor
		//	tinygltf::Texture & tex = model.textures[0];

		//	if (tex.source > -1)
		//	{

		//		GLuint texid;
		//		glGenTextures(1, &texid);

		//		tinygltf::Image & image = model.images[tex.source];

		//		glBindTexture(GL_TEXTURE_2D, texid);
		//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//		GLenum format = GL_RGBA;

		//		if (image.component == 1)
		//		{
		//			format = GL_RED;
		//		}
		//		else if (image.component == 2)
		//		{
		//			format = GL_RG;
		//		}
		//		else if (image.component == 3)
		//		{
		//			format = GL_RGB;
		//		}
		//		else
		//		{
		//			// ???
		//		}

		//		GLenum type = GL_UNSIGNED_BYTE;
		//		if (image.bits == 8)
		//		{
		//			// ok
		//		}
		//		else if (image.bits == 16)
		//		{
		//			type = GL_UNSIGNED_SHORT;
		//		}
		//		else
		//		{
		//			// ???
		//		}

		//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
		//					 format, type, &image.image.at(0));
		//	}
		//}
	}
}

void Model::drawModelNodes(const tinygltf::Node & node, const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection)
{
	if ((node.mesh >= 0) && (node.mesh < model_->meshes.size()))
	{
		drawMesh(model_->meshes[node.mesh], mModel, mView, mProjection);
	}
	for (size_t i = 0; i < node.children.size(); i++)
	{
		drawModelNodes(model_->nodes[node.children[i]], mModel, mView, mProjection);
	}
}

void Model::drawMesh(const tinygltf::Mesh & mesh, const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection)
{
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model_->accessors[primitive.indices];

		QOpenGLBuffer & vbo = vbos_[indexAccessor.bufferView];
		vbo.bind();

		program_->bind();

		program_->setUniformValue("model", mModel);
		program_->setUniformValue("view", mView);
		program_->setUniformValue("projection", mProjection);

		glDrawElements(primitive.mode, indexAccessor.count,
					   indexAccessor.componentType,
					   BUFFER_OFFSET(indexAccessor.byteOffset));
	}
}
