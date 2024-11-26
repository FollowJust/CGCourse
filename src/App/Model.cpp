#include "Model.h"

#include <string>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>


Model::Model()
{
	gltfModel_ = nullptr;
}

Model::~Model() 
{
	if (gltfModel_)
	{
		delete gltfModel_;
	}
}

bool Model::load(const QString & path)
{
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	gltfModel_ = new tinygltf::Model();
	bool res = loader.LoadBinaryFromFile(gltfModel_, &err, &warn, path.toStdString());

	if (!err.empty())
	{
		qDebug() << "Model::Load(" << path << ") ERROR: " << QString::fromStdString(err);
	}

	if (!warn.empty())
	{
		qDebug() << "Model::Load(" << path << ") WARNING: " << QString::fromStdString(warn);
	}

	if (!res) {
		delete gltfModel_;
		return false;
	}

	return true;
}

void Model::bind()
{
	assert(gltfModel_ && "Model isn't loaded. Model should be loaded before binding");

	vao_.create();
	vao_.bind();

	QMap<int, QOpenGLBuffer> vbos;

	const tinygltf::Scene & scene = gltfModel_->scenes[gltfModel_->defaultScene];
	for (const auto & node : scene.nodes) 
	{
		bindModelNodes(*gltfModel_, gltfModel_->nodes[node], vbos);
	}

	// some code to clear up in example??
}

void Model::bindMesh(const tinygltf::Model & mdl, const tinygltf::Mesh & mesh, QMap<int, QOpenGLBuffer> & vbos)
{

}

void Model::bindModelNodes(const tinygltf::Model & mdl, const tinygltf::Node & node, QMap<int, QOpenGLBuffer> & vbos)
{
	if (node.mesh >= 0 && node.mesh < mdl.meshes.size()) {
		bindMesh(mdl, mdl.meshes[node.mesh], vbos);
	}

	// traverse recursively
	for (const auto & child: node.children) {
		if (child >= 0 && (child < mdl.meshes.size()))
		{
			bindModelNodes(mdl, mdl.nodes[child], vbos);
		}
	}
}


//QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
//QOpenGLVertexArrayObject vao_;