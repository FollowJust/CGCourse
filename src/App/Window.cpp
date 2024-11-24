#include "Window.h"

#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QVBoxLayout>
#include <QColorDialog>

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
	-1.f, 3.f};
constexpr std::array<GLuint, 3u> indices = {0, 1, 2};

}// namespace

// respectfully taken from GPGPU homeworks :)
namespace MandelbrotParams
{

constexpr QVector2D center = QVector2D(-0.789136f, -0.150316f);
constexpr float sizeX = 0.00239f;
constexpr unsigned int iteration = 256;

}// namespace MandelbrotParams

QSpinBox * initIntParamWidget(QBoxLayout * parent, const QString & name)
{
	auto hBox = new QHBoxLayout();
	hBox->setSpacing(0);
	hBox->setAlignment(Qt::AlignLeft);

	auto label = new QLabel(name);
	label->setStyleSheet("QLabel { color : white; }");

	auto spinBox = new QSpinBox();

	hBox->addWidget(label);
	hBox->addWidget(spinBox);
	parent->addLayout(hBox);

	return spinBox;
}

QDoubleSpinBox * initDoubleParamWidget(QBoxLayout * parent, const QString & name)
{
	auto hBox = new QHBoxLayout();
	hBox->setSpacing(0);
	hBox->setAlignment(Qt::AlignLeft);

	auto label = new QLabel(name);
	label->setStyleSheet("QLabel { color : white; }");

	auto doubleSpinBox = new QDoubleSpinBox();
	doubleSpinBox->setDecimals(3);
	doubleSpinBox->setSingleStep(0.005f);

	hBox->addWidget(label);
	hBox->addWidget(doubleSpinBox);
	parent->addLayout(hBox);

	return doubleSpinBox;
}

Window::Window() noexcept
{
	const auto formatFPS = [](const auto value) {
		return QString("FPS: %1").arg(QString::number(value));
	};

	auto fps = new QLabel(formatFPS(0), this);
	fps->setStyleSheet("QLabel { color : white; }");

	auto layout = new QVBoxLayout();
	layout->addWidget(fps, 1);

	zoomSpinBox_ = initDoubleParamWidget(layout, "Zoom Speed");
	zoomSpinBox_->setRange(1.005f, 10.0f);

	{
		mandelbrotcenterXSpinBox_ = initDoubleParamWidget(layout, "Mandelbrot Center X");
		mandelbrotcenterXSpinBox_->setDecimals(5);
		mandelbrotcenterXSpinBox_->setSingleStep(0.0005f);
		mandelbrotcenterXSpinBox_->setRange(-1000.0f, 1000.0f);
		mandelbrotcenterXSpinBox_->setValue(MandelbrotParams::center.x());

		mandelbrotcenterYSpinBox_ = initDoubleParamWidget(layout, "Mandelbrot Center Y");
		mandelbrotcenterYSpinBox_->setDecimals(5);
		mandelbrotcenterYSpinBox_->setSingleStep(0.0005f);
		mandelbrotcenterYSpinBox_->setRange(-1000.0f, 1000.0f);
		mandelbrotcenterYSpinBox_->setValue(MandelbrotParams::center.y());
	}

	mandelbrotSizeXSpinBox_ = initDoubleParamWidget(layout, "Mandelbrot Size X");
	mandelbrotSizeXSpinBox_->setDecimals(5);
	mandelbrotSizeXSpinBox_->setSingleStep(0.0005f);
	mandelbrotSizeXSpinBox_->setRange(MandelbrotParams::sizeX, 1.0f);
	mandelbrotSizeXSpinBox_->setValue(MandelbrotParams::sizeX);

	mandelbrotIterationsSpinBox_ = initIntParamWidget(layout, "Mandelbrot Iterations");
	mandelbrotIterationsSpinBox_->setMaximum(2048);
	mandelbrotIterationsSpinBox_->setValue(MandelbrotParams::iteration);

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
	const float mandelbrotSizeX = mandelbrotSizeXSpinBox_->value();
	const auto & mandelbrotSize = QVector2D(mandelbrotSizeX, mandelbrotSizeX * aspectRatio);

	const auto & mandelbrotCenter = QVector2D(mandelbrotcenterXSpinBox_->value(), mandelbrotcenterYSpinBox_->value());
	const auto & mandelbrotStart = mandelbrotCenter - mandelbrotSize / 2.0f;

	program_->setUniformValue("orthoProjection", orthoProjection_);
	program_->setUniformValue("screenResolution", screenResolution);
	program_->setUniformValue("mandelbrotStart", mandelbrotStart);
	program_->setUniformValue("mandelbrotSize", mandelbrotSize);
	program_->setUniformValue("mandelbrotIterations", mandelbrotIterationsSpinBox_->value());
	
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
	orthoProjection_.ortho(left_, right_, bottom_, top_, 0.0f, 100.0f);
}

void Window::mousePressEvent(QMouseEvent * e)
{
	mousePressPos_ = QVector2D(e->localPos());
}

void Window::mouseReleaseEvent(QMouseEvent *)
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

	if (numDegrees.y() == 0)
	{
		return;
	}

	float t = zoomSpinBox_->value();
	float zoom = 1.0f / t;
	if (numDegrees.y() < 0.0f)
	{
		zoom = t;
	}

	const QVector2D & mousePos = QVector2D(e->position().x(), height_ - e->position().y());

	const QVector4D & wsPos = QVector4D(mousePos, 0.0f, 1.0f);

	QMatrix4x4 scaledOrthoProjection = orthoProjection_;
	scaledOrthoProjection.scale(zoom);

	const QVector4D & screenPosScaled = scaledOrthoProjection * wsPos;

	bool invertible = false;
	const QMatrix4x4 & invOrthoProjection = orthoProjection_.inverted(&invertible);
	assert(invertible);

	const QVector4D & scaledWsPos = invOrthoProjection * screenPosScaled;

	const QVector2D & wsDiff = wsPos.toVector2D() - scaledWsPos.toVector2D();

	orthoProjection_.translate(wsDiff);
	orthoProjection_.scale(zoom);
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
