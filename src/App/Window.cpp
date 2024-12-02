#include "Window.h"

#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QVBoxLayout>
#include <qcursor.h>

#include <array>

#include "Camera.h"
#include "Model.h"

namespace
{
	//todo fix that
constexpr char modelPath[] = "Duck.glb";

}// namespace

Window::Window() noexcept
{
	const auto formatFPS = [](const auto value) {
		return QString("FPS: %1").arg(QString::number(value));
	};

	auto fps = new QLabel(formatFPS(0), this);
	fps->setStyleSheet("QLabel { color : white; }");

	auto layout = new QVBoxLayout();
	layout->addWidget(fps, 1);

	setLayout(layout);

	timer_.start();

	connect(this, &Window::updateUI, [=] {
		fps->setText(formatFPS(ui_.fps));
	});

	camera_ = std::make_unique<Camera>();
}

Window::~Window()
{
	{
		// Free resources with context bounded.
		const auto guard = bindContext();
	}
}

void Window::onInit()
{
	mdl_ = std::make_unique<Model>();
	mdl_->load(modelPath);
	mdl_->bind();

	// Еnable depth test and face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
	// Update camera
	camera_->update();

	const auto guard = captureMetrics();

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate MVP matrix
	model_.setToIdentity();
	//model_.scale(0.01, 0.01, 0.01);

	mdl_->draw(model_, camera_->GetViewMatrix(), projection_);

	++frameCount_;

	// Request redraw if animated
	if (animated_)
	{
		update();
	}
}

void Window::onResize(const size_t width, const size_t height)
{
	// Configure viewport
	glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

	// Configure matrix
	const auto aspect = static_cast<float>(width) / static_cast<float>(height);
	const auto zNear = 0.1f;
	const auto zFar = 100.0f;
	const auto fov = 60.0f;
	projection_.setToIdentity();
	projection_.perspective(fov, aspect, zNear, zFar);

	setMouseTracking(true);
	QCursor::setPos(mapToGlobal(rect().center()));
	prevMousePosition_ = QVector2D(width * 0.5f, height * 0.5f);
}

void Window::mousePressEvent(QMouseEvent * e)
{
	prevMousePosition_ = QVector2D(e->pos().x(), height() - e->pos().y());
}

void Window::mouseReleaseEvent(QMouseEvent * e)
{
}

void Window::mouseMoveEvent(QMouseEvent * e)
{
	// fix out of window move (out of focus)

	const QVector2D & currentMousePosition = QVector2D(e->pos().x(), height() - e->pos().y());

	if (prevMousePosition_ == QVector2D(-1.0f, -1.0f)) {
		prevMousePosition_ = currentMousePosition;
		return;
	}

	QVector2D diff = currentMousePosition - prevMousePosition_;
	prevMousePosition_ = currentMousePosition;

	const float sensitivity = 0.7f;
	diff *= sensitivity;

	camera_->mouseMove(diff);
}

void Window::keyPressEvent(QKeyEvent * e)
{
	switch (e->key())
	{
		case Qt::Key_W: {
			camera_->startMoving(Camera::Movement::FORWARD);
			break;
		}
		case Qt::Key_S: {
			camera_->startMoving(Camera::Movement::BACKWARD);
			break;
		}
		case Qt::Key_A: {
			camera_->startMoving(Camera::Movement::LEFT);
			break;
		}
		case Qt::Key_D: {
			camera_->startMoving(Camera::Movement::RIGHT);
			break;
		}

		case Qt::Key_Control: {
			if (!mouseGrabbed_)
			{
				grabMouse();
				mouseGrabbed_ = true;
				prevMousePosition_ = QVector2D(-1.0f, -1.0f);
			}
			else {
				releaseMouse();
				mouseGrabbed_ = false;
				prevMousePosition_ = QVector2D(-1.0f, -1.0f);
			}
			break;
		}

		case Qt::Key_Space: {
			camera_->reset();
			break;
		}
		default:
			break;
	}
}

void Window::keyReleaseEvent(QKeyEvent * e)
{
	switch (e->key())
	{
		case Qt::Key_W: {
			camera_->stopMoving(Camera::Movement::FORWARD);
			break;
		}
		case Qt::Key_S: {
			camera_->stopMoving(Camera::Movement::BACKWARD);
			break;
		}
		case Qt::Key_A: {
			camera_->stopMoving(Camera::Movement::LEFT);
			break;
		}
		case Qt::Key_D: {
			camera_->stopMoving(Camera::Movement::RIGHT);
			break;
		}
		default:
			break;
	}
}

Window::PerfomanceMetricsGuard::PerfomanceMetricsGuard(std::function<void()> callback)
	: callback_{std::move(callback)}
{
}

Window::PerfomanceMetricsGuard::~PerfomanceMetricsGuard()
{
	if (callback_)
	{
		callback_();
	}
}

auto Window::captureMetrics() -> PerfomanceMetricsGuard
{
	return PerfomanceMetricsGuard{
		[&] {
			if (timer_.elapsed() >= 1000)
			{
				const auto elapsedSeconds = static_cast<float>(timer_.restart()) / 1000.0f;
				ui_.fps = static_cast<size_t>(std::round(frameCount_ / elapsedSeconds));
				frameCount_ = 0;
				emit updateUI();
			}
		}};
}
