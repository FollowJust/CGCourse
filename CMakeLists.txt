cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(mse-gl-hw-template VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
    # disable warning for qt.
    add_compile_options(/wd5240)
    add_compile_options(/wd5054)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_subdirectory(thirdparty)

include_directories(src)

# For Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src/Base)
add_subdirectory(src/App)
