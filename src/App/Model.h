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

class Model
{
public:
	Model();
	~Model();


	bool load(const QString & path);
	void bind();
	void draw();

private:
	bool loaded() const { return gltfModel_ != nullptr; }

	void bindMesh(const tinygltf::Model & mdl, const tinygltf::Mesh & mesh, QMap<int, QOpenGLBuffer> & vbos);
	void bindModelNodes(const tinygltf::Model & mdl, const tinygltf::Node & node, QMap<int, QOpenGLBuffer> & vbos);

private:
	tinygltf::Model * gltfModel_;
	QMap<int, QOpenGLBuffer> vbos_;
	QOpenGLVertexArrayObject vao_;
};