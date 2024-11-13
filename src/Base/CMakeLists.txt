set(BASE_SRCS
        GLWidget.cpp
        GLWidget.hpp
        )

add_library(Base ${BASE_SRCS})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(Base
        PRIVATE
        Qt5::Widgets
        )

add_library(FGL::Base ALIAS Base)