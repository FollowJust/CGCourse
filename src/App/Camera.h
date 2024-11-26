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

class Camera
{
public:
	enum Movement : unsigned int
	{
		FORWARD = (1 << 0),
		BACKWARD = (1 << 1),
		LEFT = (1 << 2),
		RIGHT = (1 << 3)
	};

	Camera();

	void update();

	void mouseMove(const QVector2D & offset);

	void startMoving(const Movement & direction);
	void stopMoving(const Movement & direction);
	void reset();


	QMatrix4x4 GetViewMatrix() const { return view_; };
	QMatrix4x4 GetProjectionMatrix() const { return projection_; };

private:
	void updateCameraVectors();

private:
	QVector3D position_;
	QVector3D front_;
	QVector3D worldUp_;
	QVector3D up_;
	QVector3D right_;

	float yaw_;
	float pitch_;

	unsigned int movingFlags;
	float speed_;

	QMatrix4x4 view_;
	QMatrix4x4 projection_;
};