#include "Window.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMouseEvent>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QSpinBox>
#include <QVBoxLayout>
#include <qcursor.h>
#include <qmath.h>

#include <array>

#include "Camera.h"
#include "Model.h"

namespace
{
constexpr std::array<GLfloat, 21u> fullscreenQuadVertices = {
	/* Postion */
	-1.f, -1.f,
	3.f, -1.f,
	-1.f, 3.f};
constexpr std::array<GLuint, 3u> fullscreenQuadIndices = {0, 1, 2};

//todo fix that
constexpr char modelPath[] = "DamagedHelmet.glb";

}// namespace

QSpinBox * initIntParamWidget(QBoxLayout * parent, const QString & name)
{
	auto hBox = new QHBoxLayout();
	hBox->setSpacing(0);
	hBox->setAlignment(Qt::AlignLeft);

	auto label = new QLabel(name);
	label->setStyleSheet("QLabel { color : white; }");

	auto spinBox = new QSpinBox();
	spinBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);

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
	doubleSpinBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);

	hBox->addWidget(label);
	hBox->addWidget(doubleSpinBox);
	parent->addLayout(hBox);

	return doubleSpinBox;
}

QCheckBox * initCheckBoxParamWidget(QBoxLayout * parent, const QString & name)
{
	auto hBox = new QHBoxLayout();
	hBox->setSpacing(0);
	hBox->setAlignment(Qt::AlignLeft);

	auto label = new QLabel(name);
	label->setStyleSheet("QLabel { color : white; }");

	auto checkBox = new QCheckBox();
	checkBox->setFocusPolicy(Qt::FocusPolicy::NoFocus);

	hBox->addWidget(label);
	hBox->addWidget(checkBox);
	parent->addLayout(hBox);

	return checkBox;
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

	// Model scale
	modelScaleSpinBox_ = initDoubleParamWidget(layout, "Model Scale");
	modelScaleSpinBox_->setDecimals(3);
	modelScaleSpinBox_->setSingleStep(0.01f);
	modelScaleSpinBox_->setRange(0.001f, 100.0f);
	if (QString(modelPath) == "Duck.glb")
	{
		// just to make it fit by default
		modelScaleSpinBox_->setValue(0.01f);
	}
	else
	{
		modelScaleSpinBox_->setValue(1.0f);
	}
	modelScaleSpinBox_->setFocusPolicy(Qt::FocusPolicy::NoFocus);

	// Camera fly speed
	flySpeedSpinBox_ = initDoubleParamWidget(layout, "Fly Speed");
	flySpeedSpinBox_->setDecimals(2);
	flySpeedSpinBox_->setSingleStep(0.1f);
	flySpeedSpinBox_->setRange(0.01f, 1.0f);
	flySpeedSpinBox_->setValue(0.01f);
	flySpeedSpinBox_->setFocusPolicy(Qt::FocusPolicy::NoFocus);

	// Morph params
	morphCheckBox_ = initCheckBoxParamWidget(layout, "Morph");
	morphCheckBox_->setChecked(false);

	morphSpeedSpinBox_ = initDoubleParamWidget(layout, "Morph Speed");
	morphSpeedSpinBox_->setDecimals(2);
	morphSpeedSpinBox_->setSingleStep(0.1f);
	morphSpeedSpinBox_->setRange(0.01f, 10.0f);
	morphSpeedSpinBox_->setValue(1.0f);

	morphCoefficientSpinBox_ = initDoubleParamWidget(layout, "Morph Coef");
	morphCoefficientSpinBox_->setDecimals(2);
	morphCoefficientSpinBox_->setSingleStep(0.1f);
	morphCoefficientSpinBox_->setRange(0.01f, 10.0f);
	morphCoefficientSpinBox_->setValue(1.0f);

	morphClampValueSpinBox_ = initDoubleParamWidget(layout, "Morph Clamp Value");
	morphClampValueSpinBox_->setDecimals(2);
	morphClampValueSpinBox_->setSingleStep(0.1f);
	morphClampValueSpinBox_->setRange(0.01f, 10.0f);
	morphClampValueSpinBox_->setValue(1.0f);

	// Directional Light params
	directionalLightDirectionSpinBox_ = new Utils::UIVector3D(layout, "DirLight Direction");
	directionalLightDirectionSpinBox_->setValue(QVector3D(0.0f, -1.0f, 0.0f));

	directionalLightColorSpinBox_ = new Utils::UIVector3D(layout, "DirLight Color");
	directionalLightColorSpinBox_->setValue(QVector3D(1.0f, 1.0f, 1.0f));
	directionalLightColorSpinBox_->setRange(0.0f, 1.0f);

	directionalLightAmbientCoefficientSpinBox_ = initDoubleParamWidget(layout, "DirLight Ambient");
	directionalLightAmbientCoefficientSpinBox_->setDecimals(2);
	directionalLightAmbientCoefficientSpinBox_->setSingleStep(0.1f);
	directionalLightAmbientCoefficientSpinBox_->setRange(0.0f, 10.0f);
	directionalLightAmbientCoefficientSpinBox_->setValue(0.2f);

	directionalLightSpecularCoefficientSpinBox_ = initDoubleParamWidget(layout, "DirLight Specular");
	directionalLightSpecularCoefficientSpinBox_->setDecimals(2);
	directionalLightSpecularCoefficientSpinBox_->setSingleStep(0.1f);
	directionalLightSpecularCoefficientSpinBox_->setRange(0.0f, 10.0f);
	directionalLightSpecularCoefficientSpinBox_->setValue(0.7f);

	// Spot Light params
	spotLightAttachedToCameraCheckBox_ = initCheckBoxParamWidget(layout, "SpotLight Attached to Camera");
	spotLightAttachedToCameraCheckBox_->setChecked(true);

	spotLightPositionSpinBox_ = new Utils::UIVector3D(layout, "SpotLight Position");
	spotLightPositionSpinBox_->setValue(QVector3D(0.0f, 0.0f, 0.0f));

	spotLightDirectionSpinBox_ = new Utils::UIVector3D(layout, "SpotLight Direction");
	spotLightDirectionSpinBox_->setValue(QVector3D(0.0f, 0.0f, 0.0f));


	spotLightCutOffSpinBox_ = initDoubleParamWidget(layout, "SpotLight Cut Off Angle");
	spotLightCutOffSpinBox_->setDecimals(2);
	spotLightCutOffSpinBox_->setSingleStep(0.1f);
	spotLightCutOffSpinBox_->setRange(0.0f, 90.0f);
	spotLightCutOffSpinBox_->setValue(2.5f);

	spotLightOuterCutOffSpinBox_ = initDoubleParamWidget(layout, "SpotLight Outer Cut Off Angle");
	spotLightOuterCutOffSpinBox_->setDecimals(2);
	spotLightOuterCutOffSpinBox_->setSingleStep(0.1f);
	spotLightOuterCutOffSpinBox_->setRange(0.0f, 90.0f);
	spotLightOuterCutOffSpinBox_->setValue(10.0f);

	spotLightColorSpinBox_ = new Utils::UIVector3D(layout, "SpotLight Color");
	spotLightColorSpinBox_->setValue(QVector3D(1.0f, 1.0f, 1.0f));
	spotLightColorSpinBox_->setRange(0.0f, 1.0f);

	spotLightAmbientCoefficientSpinBox_ = initDoubleParamWidget(layout, "SpotLight Ambient");
	spotLightAmbientCoefficientSpinBox_->setDecimals(2);
	spotLightAmbientCoefficientSpinBox_->setSingleStep(0.1f);
	spotLightAmbientCoefficientSpinBox_->setRange(0.0f, 10.0f);
	spotLightAmbientCoefficientSpinBox_->setValue(0.3f);
	spotLightAmbientCoefficientSpinBox_->setFocusPolicy(Qt::FocusPolicy::NoFocus);

	spotLightSpecularCoefficientSpinBox_ = initDoubleParamWidget(layout, "SpotLight Specular");
	spotLightSpecularCoefficientSpinBox_->setDecimals(2);
	spotLightSpecularCoefficientSpinBox_->setSingleStep(0.1f);
	spotLightSpecularCoefficientSpinBox_->setRange(0.0f, 10.0f);
	spotLightSpecularCoefficientSpinBox_->setValue(0.5f);
	spotLightSpecularCoefficientSpinBox_->setFocusPolicy(Qt::FocusPolicy::NoFocus);

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
	// Init fullscreen stuff
	{
		fullscreenProgram_ = std::make_unique<QOpenGLShaderProgram>();
		fullscreenProgram_->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/fullscreen.vs");
		fullscreenProgram_->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fullscreen.fs");
		fullscreenProgram_->link();
		// Create VAO object
		fsQuadVAO_.create();
		fsQuadVAO_.bind();

		// Create VBO
		fsQuadVBO_.create();
		fsQuadVBO_.bind();
		fsQuadVBO_.setUsagePattern(QOpenGLBuffer::StaticDraw);
		fsQuadVBO_.allocate(fullscreenQuadVertices.data(), static_cast<int>(fullscreenQuadVertices.size() * sizeof(GLfloat)));

		// Create IBO
		fsQuadIBO_.create();
		fsQuadIBO_.bind();
		fsQuadIBO_.setUsagePattern(QOpenGLBuffer::StaticDraw);
		fsQuadIBO_.allocate(fullscreenQuadIndices.data(), static_cast<int>(fullscreenQuadIndices.size() * sizeof(GLuint)));

		// Bind attributes
		fullscreenProgram_->bind();

		fullscreenProgram_->enableAttributeArray(0);
		fullscreenProgram_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2 * sizeof(GLfloat)));

		// Release all
		fullscreenProgram_->release();

		fsQuadVAO_.release();

		fsQuadIBO_.release();
		fsQuadVBO_.release();
	}

	model_ = std::make_unique<Model>();
	model_->load(modelPath);
	model_->bind();

	// Еnable depth test and face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear all FBO buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
	// Update camera
	camera_->setSpeed(flySpeedSpinBox_->value());
	camera_->update();

	const auto guard = captureMetrics();

	GBufferPass();

	FullscreenPass();

	//model_->setScale(modelScaleSpinBox_->value());

	//model_->bindProgram();

	//// Common uniforms
	//model_->setUniformValue("view", camera_->GetViewMatrix());
	//model_->setUniformValue("projection", projection_);

	//model_->setUniformValue("viewPos", camera_->GetViewPosition());

	//// Morph
	//if (morphCheckBox_->isChecked())
	//{
	//	model_->setUniformValue("morphingMixValue", abs(qCos(totalFramesCount / 100.0f * morphSpeedSpinBox_->value())));
	//}
	//else
	//{
	//	model_->setUniformValue("morphingMixValue", 0);
	//}
	//model_->setUniformValue("morphCoef", morphCoefficientSpinBox_->value());
	//model_->setUniformValue("morphClampValue", morphClampValueSpinBox_->value());

	//// Directional Light
	//model_->setUniformValue("directionalLight.direction", directionalLightDirectionSpinBox_->getValue());

	//model_->setUniformValue("directionalLight.color", directionalLightColorSpinBox_->getValue());
	//model_->setUniformValue("directionalLight.ambientStrength", directionalLightAmbientCoefficientSpinBox_->value());
	//model_->setUniformValue("directionalLight.specularStrength", directionalLightSpecularCoefficientSpinBox_->value());

	//// Spot Light
	//if (spotLightAttachedToCameraCheckBox_->isChecked())
	//{
	//	model_->setUniformValue("spotLight.position", camera_->GetViewPosition());
	//	model_->setUniformValue("spotLight.direction", camera_->GetViewDirection());
	//}
	//else
	//{
	//	model_->setUniformValue("spotLight.position", spotLightPositionSpinBox_->getValue());
	//	model_->setUniformValue("spotLight.direction", spotLightDirectionSpinBox_->getValue());
	//}
	//model_->setUniformValue("spotLight.cutOff", qCos(qDegreesToRadians(spotLightCutOffSpinBox_->value())));
	//model_->setUniformValue("spotLight.outerCutOff", qCos(qDegreesToRadians(spotLightOuterCutOffSpinBox_->value())));

	//model_->setUniformValue("spotLight.color", spotLightColorSpinBox_->getValue());
	//model_->setUniformValue("spotLight.ambientStrength", spotLightAmbientCoefficientSpinBox_->value());
	//model_->setUniformValue("spotLight.specularStrength", spotLightSpecularCoefficientSpinBox_->value());


	//model_->draw();

	++frameCount_;
	++totalFramesCount;

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

	// Configure framebuffers
	resizeFramebuffers(width, height);

	// Configure matrix
	const auto aspect = static_cast<float>(width) / static_cast<float>(height);
	const auto zNear = 0.1f;
	const auto zFar = 100.0f;
	const auto fov = 60.0f;
	projection_.setToIdentity();
	projection_.perspective(fov, aspect, zNear, zFar);
	
	// Reset mouse
	QCursor::setPos(mapToGlobal(rect().center()));
	prevMousePosition_ = QVector2D(width * 0.5f, height * 0.5f);
}

void Window::resizeFramebuffers(const size_t width, const size_t height)
{
	const QSize & resolution = QSize(width, height);
	if (gbufferFBO_)
	{
		if (gbufferFBO_->isBound())
		{
			QOpenGLFramebufferObject::bindDefault();
		}
		gbufferFBO_->release();
	}

	gbufferFBO_.reset(new QOpenGLFramebufferObject(resolution, QOpenGLFramebufferObject::Attachment::Depth));
	// add another color attachment for normals
	gbufferFBO_->addColorAttachment(resolution, GL_RGBA);

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLint depthBuffer;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &depthBuffer);

	//glBindTexture(GL_TEXTURE0, depthBuffer);
	qDebug() << depthBuffer << gbufferFBO_->textures().size();

	gbufferFBO_->release();
}

void Window::GBufferPass()
{
	gbufferFBO_->bind();

	QOpenGLExtraFunctions * f = QOpenGLContext::currentContext()->extraFunctions();
	GLenum bufs[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	f->glDrawBuffers(2, bufs);

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{
		model_->setScale(modelScaleSpinBox_->value());

		model_->bindProgram();

		// Common uniforms
		model_->setUniformValue("view", camera_->GetViewMatrix());
		model_->setUniformValue("projection", projection_);

		model_->draw();
	}

	gbufferFBO_->release();
}

void Window::FullscreenPass()
{
	assert(QOpenGLFramebufferObject::bindDefault());

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	fullscreenProgram_->bind();

	fsQuadVAO_.bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gbufferFBO_->textures()[1]);

	// Draw
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

	// Release VAO and shader program
	fsQuadVAO_.release();
	fullscreenProgram_->release();
}

void Window::mousePressEvent(QMouseEvent * e)
{
	prevMousePosition_ = QVector2D(e->pos().x(), height() - e->pos().y());
}

void Window::mouseMoveEvent(QMouseEvent * e)
{
	const QVector2D & currentMousePosition = QVector2D(e->pos().x(), height() - e->pos().y());

	if (prevMousePosition_ == QVector2D(-1.0f, -1.0f))
	{
		prevMousePosition_ = currentMousePosition;
		return;
	}

	QVector2D diff = currentMousePosition - prevMousePosition_;
	prevMousePosition_ = currentMousePosition;

	const float sensitivity = 0.7f;
	diff *= sensitivity;

	camera_->mouseMove(diff);
}

void Window::enterEvent(QEvent *)
{
	prevMousePosition_ = QVector2D(-1.0f, -1.0f);
}

void Window::leaveEvent(QEvent *)
{
	prevMousePosition_ = QVector2D(-1.0f, -1.0f);
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
		case Qt::Key_Q: {
			camera_->startMoving(Camera::Movement::DOWN);
			break;
		}
		case Qt::Key_E: {
			camera_->startMoving(Camera::Movement::UP);
			break;
		}
		case Qt::Key_Control: {
			if (!mouseGrabbed_)
			{
				setMouseTracking(true);
				grabMouse();

				mouseGrabbed_ = true;
				prevMousePosition_ = QVector2D(-1.0f, -1.0f);
			}
			else
			{
				setMouseTracking(false);
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
		case Qt::Key_Q: {
			camera_->stopMoving(Camera::Movement::DOWN);
			break;
		}
		case Qt::Key_E: {
			camera_->stopMoving(Camera::Movement::UP);
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

Utils::UIVector3D::UIVector3D(QBoxLayout * parent, const QString & name)
{
	auto hBox = new QHBoxLayout();
	hBox->setSpacing(0);
	hBox->setAlignment(Qt::AlignLeft);

	auto label = new QLabel(name);
	label->setStyleSheet("QLabel { color : white; }");
	hBox->addWidget(label);


	x_ = new QDoubleSpinBox();
	x_->setDecimals(3);
	x_->setSingleStep(0.01f);
	x_->setRange(-100.0f, 100.0f);
	x_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	hBox->addWidget(x_);

	y_ = new QDoubleSpinBox();
	y_->setDecimals(3);
	y_->setSingleStep(0.01f);
	y_->setRange(-100.0f, 100.0f);
	y_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	hBox->addWidget(y_);

	z_ = new QDoubleSpinBox();
	z_->setDecimals(3);
	z_->setSingleStep(0.01f);
	z_->setRange(-100.0f, 100.0f);
	z_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	hBox->addWidget(z_);

	parent->addLayout(hBox);
}

void Utils::UIVector3D::setValue(const QVector3D value)
{
	x_->setValue(value.x());
	y_->setValue(value.y());
	z_->setValue(value.z());
}

QVector3D Utils::UIVector3D::getValue() const
{
	return QVector3D(x_->value(), y_->value(), z_->value());
}

void Utils::UIVector3D::setRange(const float minValue, const float maxValue)
{
	x_->setRange(minValue, maxValue);
	y_->setRange(minValue, maxValue);
	z_->setRange(minValue, maxValue);
}

Utils::UIVector2D::UIVector2D(QBoxLayout * parent, const QString & name)
{
	auto hBox = new QHBoxLayout();
	hBox->setSpacing(0);
	hBox->setAlignment(Qt::AlignLeft);

	auto label = new QLabel(name);
	label->setStyleSheet("QLabel { color : white; }");
	hBox->addWidget(label);


	x_ = new QDoubleSpinBox();
	x_->setDecimals(3);
	x_->setSingleStep(0.01f);
	x_->setRange(-100.0f, 100.0f);
	x_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	hBox->addWidget(x_);

	y_ = new QDoubleSpinBox();
	y_->setDecimals(3);
	y_->setSingleStep(0.01f);
	y_->setRange(-100.0f, 100.0f);
	y_->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	hBox->addWidget(y_);

	parent->addLayout(hBox);
}

void Utils::UIVector2D::setValue(const QVector3D value)
{
	x_->setValue(value.x());
	y_->setValue(value.y());
}

QVector2D Utils::UIVector2D::getValue() const
{
	return QVector2D(x_->value(), y_->value());
}

void Utils::UIVector2D::setRange(const float minValue, const float maxValue)
{
	x_->setRange(minValue, maxValue);
	y_->setRange(minValue, maxValue);
}
