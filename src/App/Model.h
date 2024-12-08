#pragma once

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>

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
	void draw();

	bool loaded() const { return model_ != nullptr; };

	void setScale(const float scale);

	void bindProgram();
	void setUniformValue(const QString & uniformName, const float value);
	void setUniformValue(const QString & uniformName, const QVector3D & value);
	void setUniformValue(const QString & uniformName, const QMatrix4x4 & value);

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
