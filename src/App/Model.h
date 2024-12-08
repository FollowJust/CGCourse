#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>

#include <memory>

namespace tinygltf
{
class Model;
class Node;
class Mesh;
}// namespace tinygltf

class QOpenGLShaderProgram;
class QOpenGLTexture;
class QOpenGLBuffer;

class Model : public QOpenGLFunctions
{
public:
	Model();
	~Model();
	bool load(const QString & path);
	void bind();
	void draw(const QMatrix4x4 & mView, const QMatrix4x4 & mProjection);

	bool loaded() const { return model_ != nullptr; };

private:
	void bindModelNodes(const tinygltf::Node & node);
	void bindMesh(const tinygltf::Mesh & mesh);

	void drawModelNodes(const tinygltf::Node & node);
	void drawMesh(const tinygltf::Mesh & mesh);

private:
	tinygltf::Model * model_;

private:
	QMatrix4x4 mModel_;

	std::unique_ptr<QOpenGLShaderProgram> program_;

	std::unique_ptr<QOpenGLTexture> texture_;
	QOpenGLVertexArrayObject vao_;
	QMap<int, QOpenGLBuffer> vbos_;
};