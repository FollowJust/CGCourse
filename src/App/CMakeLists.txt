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
    Shaders/gbuffer.fs
    Shaders/gbuffer.vs
    Shaders/fullscreen.fs
    Shaders/fullscreen.vs
    Shaders/ssao.fs
    Shaders/ssao.vs
    Textures/voronoi.png
    Models/Duck.glb
    Models/DamagedHelmet.glb

    resources.qrc
)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Copy model file to binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Models/Duck.glb
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Models/DamagedHelmet.glb
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(demo-app ${SRCS})

target_link_libraries(demo-app
    PRIVATE
        Qt5::Widgets
        FGL::Base
        thirdparty::tinygltf
)