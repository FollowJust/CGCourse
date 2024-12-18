//#pragma once

#include <Base/GLWidget.hpp>

#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include <functional>
#include <memory>

class Camera;
class Model;
class Mesh;

class QOpenGLFramebufferObject;

class QBoxLayout;
class QDoubleSpinBox;
class QCheckBox;

class FrameBufferObjectWrapper;

namespace Utils
{

class UIVector2D : public QObject
{
public:
	UIVector2D(QBoxLayout * parent, const QString & name);

	void setValue(const QVector3D value);
	QVector2D getValue() const;

	void setRange(const float minValue, const float maxValue);

private:
	QDoubleSpinBox * x_;
	QDoubleSpinBox * y_;
};

class UIVector3D : public QObject
{
public:
	UIVector3D(QBoxLayout * parent, const QString & name);
	
	void setValue(const QVector3D value);
	QVector3D getValue() const;

	void setRange(const float minValue, const float maxValue);

private:
	QDoubleSpinBox * x_;
	QDoubleSpinBox * y_;
	QDoubleSpinBox * z_;
};
} // Utils

class Window final : public fgl::GLWidget
{
	Q_OBJECT
public:
	Window() noexcept;
	~Window() override;

public: // fgl::GLWidget
	void onInit() override;
	void onRender() override;
	void onResize(size_t width, size_t height) override;

private:
	void resizeFramebuffers(const QSize & resolution);
	void GBufferPass();

	void SSAOPass();

	void BlurPass();

	enum PassType
	{
		ALBEDO = 0,
		NORMALS = 1,
		SSAO,
		SSAO_BLURRED,
		FINAL
	};

	PassType currentPass_ = PassType::FINAL;

	void FullscreenPass(const PassType &type);

	void FinalPass();


private:
	void mousePressEvent(QMouseEvent * e) override;
	void mouseMoveEvent(QMouseEvent * e) override;
	void enterEvent(QEvent * e) override;
	void leaveEvent(QEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
	void keyReleaseEvent(QKeyEvent * e) override;

private:
	class PerfomanceMetricsGuard final
	{
	public:
		explicit PerfomanceMetricsGuard(std::function<void()> callback);
		~PerfomanceMetricsGuard();

		PerfomanceMetricsGuard(const PerfomanceMetricsGuard &) = delete;
		PerfomanceMetricsGuard(PerfomanceMetricsGuard &&) = delete;

		PerfomanceMetricsGuard & operator=(const PerfomanceMetricsGuard &) = delete;
		PerfomanceMetricsGuard & operator=(PerfomanceMetricsGuard &&) = delete;

	private:
		std::function<void()> callback_;
	};

private:
	[[nodiscard]] PerfomanceMetricsGuard captureMetrics();

signals:
	void updateUI();

private:
	size_t width_, height_;
	float aspect_, fov_;
	QMatrix4x4 projection_;

	QElapsedTimer timer_;
	size_t frameCount_ = 0;

	struct {
		size_t fps = 0;
	} ui_;

	bool animated_ = true;

private:
	// We can reuse them for all fullscreen passes
	QOpenGLVertexArrayObject fsQuadVAO_;
	QOpenGLBuffer fsQuadVBO_{QOpenGLBuffer::Type::VertexBuffer};
	QOpenGLBuffer fsQuadIBO_{QOpenGLBuffer::Type::IndexBuffer};

	std::unique_ptr<QOpenGLShaderProgram> ssaoProgram_;
	QVector<QVector3D> kernels_;

	std::unique_ptr<QOpenGLShaderProgram> blurProgram_;
	std::unique_ptr<QOpenGLShaderProgram> finalProgram_;
	std::unique_ptr<QOpenGLShaderProgram> fullscreenProgram_;

	std::unique_ptr<FrameBufferObjectWrapper> gbufferFBO_;
	std::unique_ptr<FrameBufferObjectWrapper> ssaoFBO_;
	std::unique_ptr<FrameBufferObjectWrapper> blurFBO_;

private:
	size_t totalFramesCount = 0;

private:
	std::unique_ptr<Camera> camera_;
	
	std::unique_ptr<Model> model_;
	QVector2D prevMousePosition_ = QVector2D(-1.0f, -1.0f);
	bool mouseGrabbed_ = false;

private:
	QDoubleSpinBox * flySpeedSpinBox_;

	Utils::UIVector3D * directionalLightDirectionSpinBox_;
	Utils::UIVector3D * directionalLightColorSpinBox_;
	QDoubleSpinBox * directionalLightSpecularCoefficientSpinBox_;

	QDoubleSpinBox * spotLightCutOffSpinBox_;
	QDoubleSpinBox * spotLightOuterCutOffSpinBox_;
	Utils::UIVector3D * spotLightColorSpinBox_;
	QDoubleSpinBox * spotLightSpecularCoefficientSpinBox_;
};
