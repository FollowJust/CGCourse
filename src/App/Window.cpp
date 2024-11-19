#include "Window.h"

#include <QMouseEvent>
#include <QLabel>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVBoxLayout>
#include <QScreen>

#include <array>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>

namespace
{

constexpr std::array<GLfloat, 21u> vertices = {
	/* Postion */
	-1.f, -1.f,
	3.f, -1.f,
	-1.f, 3.f
};
constexpr std::array<GLuint, 3u> indices = {0, 1, 2};

}// namespace

namespace MandelbrotParams
{
constexpr QVector2D center = QVector2D(-0.789136f, -0.150316f);
constexpr float sizeX = 0.00239f;
constexpr unsigned int iteration = 256;
}// namespace MandelbrotParams

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
}

Window::~Window()
{
	{
		// Free resources with context bounded.
		const auto guard = bindContext();
		program_.reset();
	}
}

void Window::onInit()
{
	// Configure shaders
	program_ = std::make_unique<QOpenGLShaderProgram>(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/mandelbrot.vs");
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment,
									  ":/Shaders/mandelbrot.fs");
	program_->link();

	// Create VAO object
	vao_.create();
	vao_.bind();

	// Create VBO
	vbo_.create();
	vbo_.bind();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

	// Create IBO
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

	// Bind attributes
	program_->bind();

	program_->enableAttributeArray(0);
	program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

	// Release all
	program_->release();

	vao_.release();

	ibo_.release();
	vbo_.release();

	// Еnable depth test and face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
	const auto guard = captureMetrics();

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind VAO and shader program
	program_->bind();

	const auto & screenResolution = QVector2D(width_, height_);
	const float aspectRatio = screenResolution.y() / screenResolution.x();
	const auto & mandelbrotSize = QVector2D(MandelbrotParams::sizeX, MandelbrotParams::sizeX * aspectRatio);
	const auto & mandelbrotStart = MandelbrotParams::center - mandelbrotSize / 2.0f;

	program_->setUniformValue("orthoProjection", orthoProjection_);
	program_->setUniformValue("screenResolution", screenResolution);
	program_->setUniformValue("mandelbrotStart", mandelbrotStart);
	program_->setUniformValue("mandelbrotSize", mandelbrotSize);
	program_->setUniformValue("mandelbrotIterations", MandelbrotParams::iteration);

	vao_.bind();

	// Draw
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

	// Release VAO and shader program
	vao_.release();
	program_->release();

	++frameCount_;

	update();
}

void Window::onResize(const size_t width, const size_t height)
{
	width_ = width;
	height_ = height;

	// Configure viewport
	glViewport(0, 0, static_cast<GLint>(width_), static_cast<GLint>(height_));

	// Configure orthographic projection matrix
	left_ = -1 * (float)width_ * 0.5f;
	right_ = (float)width_ * 0.5f;
	bottom_ = -1 * (float)height_ * 0.5f;
	top_ = (float)height_ * 0.5f;

	orthoProjection_.setToIdentity();
	orthoProjection_.ortho(left_, right_, bottom_, top_, 0.1f, 100.0f);
}

void Window::mousePressEvent(QMouseEvent * e)
{
	mousePressPos_ = QVector2D(e->localPos());
}

void Window::mouseReleaseEvent(QMouseEvent * e)
{
	mousePressPos_ = QVector2D(0.0f, 0.0f);
}

void Window::mouseMoveEvent(QMouseEvent * e)
{
	QVector2D diff = QVector2D(e->localPos()) - mousePressPos_;
	if (diff != QVector2D(0.0f, 0.0f))
	{
		orthoProjection_.translate({-1 * diff.x(), diff.y(), 0.0f});
	}
	mousePressPos_ = QVector2D(e->localPos());
}

void Window::wheelEvent(QWheelEvent * e)
{
	QPoint numDegrees = e->angleDelta() / 8;

	if (numDegrees.y() == 0) {
		return;
	}

	float t = 1.005f;
	float zoom = 1.0f / t;
	if (numDegrees.y() <= 0.0f) {
		zoom = t;
	}

	const auto & screenResolution = QVector2D(width_, height_);

	QVector2D wsPos = QVector2D(e->position());

	QVector2D screenPos = (orthoProjection_ * QVector4D(wsPos, 0.0f, 1.0f)).toVector2D();

	QMatrix4x4 scaledOrthoProjection = orthoProjection_;
	scaledOrthoProjection.scale(zoom);

	QVector2D screenPosScaled = (scaledOrthoProjection * QVector4D(wsPos, 0.0f, 1.0f)).toVector2D();


	bool invertible = false;
	QMatrix4x4 invOrthoProjection = orthoProjection_.inverted(&invertible);
	assert(invertible);

	QMatrix4x4 invScaledOrthoProjection = scaledOrthoProjection.inverted(&invertible);
	assert(invertible);

	QVector2D wsPosScaled = (invOrthoProjection * QVector4D(screenPosScaled, 0.0f, 1.0f)).toVector2D();
	QVector2D wsPosScaled2 = (QVector4D(screenPos, 0.0f, 1.0f) * invScaledOrthoProjection).toVector2D();

	QVector2D diff = wsPos - wsPosScaled2;
	qDebug() << wsPos << '\t' << screenPos << '\t' << screenPosScaled << '\t' << wsPos << '\t' << wsPosScaled << '\t' << wsPosScaled2 << '\n';
	//qDebug() << screenPos - screenPosScaled << '\t' << wsPos - wsPosScaled << '\t' << wsPos - wsPosScaled2 << '\n';
	orthoProjection_.translate({diff.x(), diff.y(), 0.0f});
	orthoProjection_.scale(zoom);
}

Window::PerfomanceMetricsGuard::PerfomanceMetricsGuard(std::function<void()> callback)
	: callback_{ std::move(callback) }
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
		}
	};
}
