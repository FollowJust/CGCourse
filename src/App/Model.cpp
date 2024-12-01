#include "Model.h"

#include <string>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>


//Model::Model()
//{
//	gltfModel_ = nullptr;
//	program_ = std::make_unique<QOpenGLShaderProgram>();
//	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
//	program_->addShaderFromSourceFile(QOpenGLShader::Fragment,
//									  ":/Shaders/diffuse.fs");
//	program_->link();
//
//	binded_ = false;
//}
//
//Model::~Model() 
//{
//	if (gltfModel_)
//	{
//		delete gltfModel_;
//	}
//}
//
//bool Model::load(const QString & path)
//{
//	tinygltf::TinyGLTF loader;
//	std::string err, warn;
//
//	gltfModel_ = new tinygltf::Model();
//	bool res = loader.LoadBinaryFromFile(gltfModel_, &err, &warn, path.toStdString());
//
//	if (!err.empty())
//	{
//		qDebug() << "Model::Load(" << path << ") ERROR: " << QString::fromStdString(err);
//	}
//
//	if (!warn.empty())
//	{
//		qDebug() << "Model::Load(" << path << ") WARNING: " << QString::fromStdString(warn);
//	}
//
//	if (!res) {
//		delete gltfModel_;
//		return false;
//	}
//
//	return true;
//}
//
//void Model::bind()
//{
//	assert(loaded() && "Model isn't loaded. Model should be loaded before binding");
//
//	vao_.create();
//	vao_.bind();
//
//	const tinygltf::Scene & scene = gltfModel_->scenes[gltfModel_->defaultScene];
//	for (const auto & node : scene.nodes) 
//	{
//		bindModelNodes(gltfModel_->nodes[node]);
//	}
//
//	binded_ = true;
//}
//
//void Model::draw()
//{
//	assert(loaded() && binded() && "Model isn't loaded or binded. Model should be loaded and binded before being drawn");
//
//	vao_.bind();
//
//	const tinygltf::Scene & scene = gltfModel_->scenes[gltfModel_->defaultScene];
//
//	for (const auto & node: scene.nodes)
//	{
//		drawModelNodes(gltfModel_->nodes[node]);
//	}
//
//	// unbind?
//	vao_.release();
//	program_->release();
//}
//
//void Model::bindMesh(const tinygltf::Mesh & mesh)
//{
//	for (size_t i = 0; i < gltfModel_->bufferViews.size(); ++i)
//	{
//		const tinygltf::BufferView & bufferView = gltfModel_->bufferViews[i];
//		if (bufferView.target == 0)
//		{
//			qDebug() << "Skipping, because bufferView.target is 0";
//			continue;
//		}
//
//		const tinygltf::Buffer & buffer = gltfModel_->buffers[bufferView.buffer];
//
//		QOpenGLBuffer::Type bufferType;
//		switch (bufferView.target)
//		{
//			case GL_ARRAY_BUFFER: {
//				bufferType = QOpenGLBuffer::Type::VertexBuffer;
//				break;
//			}
//			case GL_ELEMENT_ARRAY_BUFFER: {
//				bufferType = QOpenGLBuffer::Type::IndexBuffer;
//				break;
//			}
//		}
//
//		auto vbo = vbos_.insert(static_cast<int>(i), QOpenGLBuffer(bufferType));
//		
//		vbo->create();
//		vbo->bind();
//		vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
//		vbo->allocate(&buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
//	}
//
//	program_->bind();
//
//	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
//		const tinygltf::Primitive & prim = mesh.primitives[i];
//		const tinygltf::Accessor & indexAccessor = gltfModel_->accessors[prim.indices];
//
//		for (auto& attrib : prim.attributes) {
//			const tinygltf::Accessor & accessor = gltfModel_->accessors[attrib.second];
//			const int byteStride = accessor.byteOffset;
//
//			vbos_[accessor.bufferView].bind();
//
//			int size = 1;
//			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
//				size = accessor.type;
//			}
//
//			int vaa = -1;
//
//			if (!attrib.first.compare("POSITION"))
//			{
//				vaa = 0;
//			}
//
//			if (!attrib.first.compare("NORMAL"))
//			{
//				vaa = 1;
//			}
//
//			if (!attrib.first.compare("TEXCOORD_0"))
//			{
//				vaa = 2;
//			}
//
//			if (vaa > -1) {
//				program_->enableAttributeArray(vaa);
//				program_->setAttributeBuffer(vaa, accessor.componentType, accessor.byteOffset, size, byteStride);
//			}
//			else {
//				qDebug() << "Unresolved vaa=" << vaa << "(" << QString::fromStdString(attrib.first) << ", " << attrib.second << ")";
//			}
//		}
//	}
//}
//
//void Model::bindModelNodes(const tinygltf::Node & node)
//{
//	if (node.mesh >= 0 && node.mesh < gltfModel_->meshes.size())
//	{
//		bindMesh(gltfModel_->meshes[node.mesh]);
//	}
//
//	// traverse recursively
//	for (const auto & child: node.children) {
//		if (child >= 0 && (child < gltfModel_->meshes.size()))
//		{
//			bindModelNodes(gltfModel_->nodes[child]);
//		}
//	}
//}
//
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//void Model::drawMesh(const tinygltf::Mesh & mesh)
//{
//	for (size_t i = 0; i < mesh.primitives.size(); ++i) {
//		const tinygltf::Primitive & prim = mesh.primitives[i];
//		const tinygltf::Accessor & indexAccessor = gltfModel_->accessors[prim.indices];
//
//		vbos_[indexAccessor.bufferView].bind();
//
//		glDrawElements(static_cast<GLenum>(prim.mode), static_cast<GLsizei>(indexAccessor.count), static_cast<GLenum>(indexAccessor.componentType), BUFFER_OFFSET(indexAccessor.byteOffset));
//	}
//}
//
//void Model::drawModelNodes(const tinygltf::Node & node)
//{
//	if (node.mesh >= 0 && node.mesh < gltfModel_->meshes.size()) 
//	{
//		drawMesh(gltfModel_->meshes[node.mesh]);
//	}
//
//	// traverse recursively
//	for (const auto & child: node.children)
//	{
//		if (child >= 0 && (child < gltfModel_->meshes.size()))
//		{
//			drawModelNodes(gltfModel_->nodes[child]);
//		}
//	}
//}

QVector<Mesh::Vertex> Mesh::defaultVertices()
{
	return {
		Vertex{
			{-0.5f, -0.5f, 0.f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f}
		},
		Vertex{
			{0.5f, -0.5f, 0.f},
			{0.0f, 0.0f, 1.0f},
			{0.5f, 1.0f}
		},
		Vertex{
			{0.0f, 0.5f, 0.f},
			{0.0f, 0.0f, 1.0f},
			{1.0f, 0.0f}
		}
	};
}

Mesh::Mesh()
{
	initializeOpenGLFunctions();
}

void Mesh::init(const QVector<Vertex> & vertices, const QVector<unsigned int> & indices)
{
	// todo move that
	program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/diffuse.vs");
	program_.addShaderFromSourceFile(QOpenGLShader::Fragment,
									  ":/Shaders/diffuse.fs");
	program_.link();

	vao_.create();
	vao_.bind();

	vbo_.create();
	vbo_.bind();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.allocate(vertices_.data(), static_cast<int>(vertices_.size() * sizeof(Vertex)));

	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices_.data(), static_cast<int>(indices_.size() * sizeof(GLuint)));

	program_.bind();

	// position
	program_.enableAttributeArray(0);
	program_.setAttributeBuffer(0, GL_FLOAT, static_cast<int>(offsetof(Vertex, position)), 3, static_cast<int>(sizeof(Vertex)));

	// normal
	program_.enableAttributeArray(1);
	program_.setAttributeBuffer(1, GL_FLOAT, static_cast<int>(offsetof(Vertex, normal)), 3, static_cast<int>(sizeof(Vertex)));

	// uv
	program_.enableAttributeArray(2);
	program_.setAttributeBuffer(2, GL_FLOAT, static_cast<int>(offsetof(Vertex, uv)), 2, static_cast<int>(sizeof(Vertex)));

	program_.release();

	vao_.release();

	ibo_.release();
	vbo_.release();
}

void Mesh::draw(const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection)
{
	program_.bind();
	vao_.bind();

	// update uniforms
	program_.setUniformValue("model", mModel);
	program_.setUniformValue("view", mView);
	program_.setUniformValue("projection", mProjection);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);

	vao_.release();
	program_.release();
}

//bool RealModel::load(const QString & path)
//{
//	tinygltf::TinyGLTF loader;
//	std::string err, warn;
//
//	tinygltf::Model model;
//	bool res = loader.LoadBinaryFromFile(&model, &err, &warn, path.toStdString());
//
//	if (!err.empty())
//	{
//		qDebug() << "Model::Load(" << path << ") ERROR: " << QString::fromStdString(err);
//	}
//
//	if (!warn.empty())
//	{
//		qDebug() << "Model::Load(" << path << ") WARNING: " << QString::fromStdString(warn);
//	}
//
//	if (!res)
//	{
//		return false;
//	}
//
//	return parseModel(model);
//}
//
//
//void RealModel::draw(const std::unique_ptr<QOpenGLShaderProgram> & program, const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection)
//{
//	for (auto mesh : meshes_) {
//		mesh->draw(mModel, mView, mProjection);
//	}
//}
//
//bool RealModel::parseModel(const tinygltf::Model & model)
//{
//	const tinygltf::Scene & scene = model.scenes[model.defaultScene];
//	for (const auto & node: scene.nodes)
//	{
//		if (!parseNode(model, model.nodes[node])) {
//			return false;
//		}
//	}
//
//	return true;
//}
//
//bool RealModel::parseNode(const tinygltf::Model & model, const tinygltf::Node & node)
//{
//	if (node.mesh >= 0 && node.mesh < model.meshes.size())
//	{
//		return parseMesh(model, model.meshes[node.mesh]);
//	}
//
//	// traverse recursively
//	for (const auto & child: node.children)
//	{
//		if (child >= 0 && (child < model.meshes.size()))
//		{
//			if (!parseNode(model, model.nodes[child]))
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
//bool RealModel::parseMesh(const tinygltf::Model & model, const tinygltf::Mesh & mesh)
//{
//	QVector<Mesh::Vertex> vertices;
//	QVector<unsigned int> indices;
//
//
//	for (const auto & primitive: mesh.primitives)
//	{
//		
//	}
//
//	return true;
//}
