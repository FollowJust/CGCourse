#pragma once

//todo remove unnessecary
#include <Base/GLWidget.hpp>

#include <QElapsedTimer>
#include <QVector>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <functional>
#include <memory>

namespace tinygltf
{
class Model;
class Node;
class Mesh;
}

class Mesh : protected QOpenGLFunctions
{
public:
	struct Vertex {
		QVector3D position;
		QVector3D normal;
		QVector2D uv;
	};

	static QVector<Vertex> defaultVertices();
	static QVector<unsigned int> defaultIndices() { return {0, 1, 2}; }

public:
	Mesh();
	
	void init(const QVector<Vertex> & vertices, const QVector<unsigned int> & indices);
	void draw(const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection);

private:
	QVector<Vertex> vertices_;
	QVector<unsigned int> indices_;

private:
	QOpenGLShaderProgram program_;
	QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
	QOpenGLVertexArrayObject vao_;
};

//class RealModel
//{
//public:
//	RealModel();
//	~RealModel();
//	
//
//	bool load(const QString & path);
//	void draw(const std::unique_ptr<QOpenGLShaderProgram> & program, const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection);
//
//private:
//	bool parseModel(const tinygltf::Model & model);
//	bool parseNode(const tinygltf::Model & model, const tinygltf::Node & node);
//	bool parseMesh(const tinygltf::Model & model, const tinygltf::Mesh & mesh);
//
//private:
//	QVector<QSharedPointer<Mesh>> meshes_;
//};
//
//class Model : protected QOpenGLFunctions
//{
//public:
//	Model();
//	~Model();
//
//
//	bool load(const QString & path);
//	void bind();
//	void draw();
//
//private:
//	bool loaded() const { return gltfModel_ != nullptr; }
//	bool binded() const { return binded_; }
//
//	void bindMesh(const tinygltf::Mesh & mesh);
//	void bindModelNodes(const tinygltf::Node & node);
//
//	void drawMesh(const tinygltf::Mesh & mesh);
//	void drawModelNodes(const tinygltf::Node & node);
//
//private:
//	tinygltf::Model * gltfModel_;
//
//	bool binded_;
//
//	std::unique_ptr<QOpenGLShaderProgram> program_;
//	QMap<int, QOpenGLBuffer> vbos_;
//	QOpenGLVertexArrayObject vao_;
//};