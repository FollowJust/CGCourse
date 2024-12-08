set(SRCS
    main.cpp
    Window.cpp
    Window.h
    Camera.cpp
    Camera.h
    Model.cpp
    Model.h

    Shaders/diffuse.fs
    Shaders/diffuse.vs
    Textures/voronoi.png
    Models/Duck.glb

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