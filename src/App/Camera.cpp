#include "Camera.h"

#include <qmath.h>

Camera::Camera()
{
	reset();
}

void Camera::update()
{
	updateCameraVectors();

	if (movingFlags & Movement::FORWARD) {
		position_ += speed_ * front_;
	}

	if (movingFlags & Movement::BACKWARD)
	{
		position_ -= speed_ * front_;
	}

	if (movingFlags & Movement::LEFT)
	{
		position_ -= speed_ * right_;
	}

	if (movingFlags & Movement::RIGHT)
	{
		position_ += speed_ * right_;
	}

	view_.setToIdentity();
	view_.lookAt(position_, position_ + front_, up_);
}

void Camera::mouseMove(const QVector2D & offset)
{
	yaw_ += offset.x();
	pitch_ += offset.y();

	if (pitch_ > 89.0f) {
		pitch_ = 89.0f;
	}
	if (pitch_ < -89.0f) {
		pitch_ = -89.0f;
	}
}

void Camera::startMoving(const Movement & direction)
{
	movingFlags |= direction;
}

void Camera::stopMoving(const Movement & direction)
{
	movingFlags &= ~direction;
}

void Camera::reset()
{
	position_ = QVector3D(0.0f, 0.0f, 3.0f);
	QVector3D target = QVector3D(0.0f, 0.0f, 0.0f);

	worldUp_ = QVector3D(0.0f, 1.0f, 0.0f);
	up_ = QVector3D(0.0f, 1.0f, 0.0f);
	front_ = QVector3D(0.0f, 0.0f, -1.0f);

	right_ = QVector3D::crossProduct(up_, front_);
	right_.normalize();


	view_.setToIdentity();
	view_.lookAt(position_, position_ + front_, up_);
	qDebug() << "Camera View:\n"
			 << view_;

	movingFlags = 0;
	speed_ = 0.005f;

	yaw_ = -90.0f;
	pitch_ = 0.0f;
}

void Camera::updateCameraVectors()
{
	front_ = QVector3D(
		qCos(qDegreesToRadians(yaw_)) * qCos(qDegreesToRadians(pitch_)),
		qSin(qDegreesToRadians(pitch_)),
		qSin(qDegreesToRadians(yaw_)) * qCos(qDegreesToRadians(pitch_))
	);

	front_.normalize();

	right_ = QVector3D::crossProduct(front_, worldUp_);
	right_.normalize();

	up_ = QVector3D::crossProduct(right_, front_);
	up_.normalize();
}
