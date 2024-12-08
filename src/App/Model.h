#pragma once

//todo remove unnessecary
#include <Base/GLWidget.hpp>

#include <QElapsedTimer>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QVector>

#include <functional>
#include <memory>

namespace tinygltf
{
class Model;
class Node;
class Mesh;
}// namespace tinygltf

class Model : public QOpenGLFunctions
{
public:
	Model();
	~Model();
	bool load(const QString & path);
	void bind();
	void draw(const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection);

	bool loaded() const { return model_ != nullptr; };

private:
	void bindModelNodes(const tinygltf::Node & node);
	void bindMesh(const tinygltf::Mesh & mesh);

	void drawModelNodes(const tinygltf::Node & node, const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection);
	void drawMesh(const tinygltf::Mesh & mesh, const QMatrix4x4 & mModel, const QMatrix4x4 & mView, const QMatrix4x4 & mProjection);

private:
	tinygltf::Model * model_;

private:
	std::unique_ptr<QOpenGLShaderProgram> program_;

	std::unique_ptr<QOpenGLTexture> texture_;
	QOpenGLVertexArrayObject vao_;
	QMap<int, QOpenGLBuffer> vbos_;
};