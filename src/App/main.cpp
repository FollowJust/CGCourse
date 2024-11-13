#include <QApplication>
#include <QSurfaceFormat>

#include "Window.h"

namespace
{
constexpr auto g_sampels = 16;
constexpr auto g_gl_major_version = 3;
constexpr auto g_gl_minor_version = 3;
}// namespace

int main(int argc, char ** argv)
{
	// Create app and set attributes.
	QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
	QApplication app(argc, argv);

	// Set default surface format.
	QSurfaceFormat format;
	format.setSamples(g_sampels);
	format.setVersion(g_gl_major_version, g_gl_minor_version);
	format.setProfile(QSurfaceFormat::CoreProfile);
	QSurfaceFormat::setDefaultFormat(format);

	// Now create window.
	Window window;
	window.resize(640, 480);
	window.show();

	return app.exec();
}