set(SRCS
    main.cpp
    Window.cpp
    Window.h

    Shaders/mandelbrot.fs
    Shaders/mandelbrot.vs
    Textures/voronoi.png

    resources.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(demo-app ${SRCS})

target_link_libraries(demo-app
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
)