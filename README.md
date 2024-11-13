![build_cmake_status](https://github.com/sadads1337/mse-gl-hw-template/actions/workflows/build_cmake.yml/badge.svg)

# ITMO MSE OpenGL homework template 2022

Qt based demo project to demonstrate how to use and implement basic 3D graphics features.
No optimizations exists. Use only for educational purposes.

## Purposes

Help students with their submission is the main goal of this repository. Do not use this code in production!!!

## Requirements

- git [https://git-scm.com](https://git-scm.com);
- C++17 compatible compiler;
- CMake 3.10+ [https://cmake.org/](https://cmake.org/);
- Qt 5 [https://www.qt.io/](https://www.qt.io/);
- (Optionally) Your favourite IDE;
- (Optionally) Ninja build [https://ninja-build.org/](https://ninja-build.org/).

## Assets

- [KhronosGroup/glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0)

## 3d-party libs

- [glm](https://github.com/g-truc/glm)
- [GSL](https://github.com/microsoft/GSL)
- [tinygltf](https://github.com/syoyo/tinygltf)

## Hardware requirements

- GPU with OpenGL 3+ support.

## Build from console

- Clone this repository `git clone <url> <path>`;
- Go to root folder `cd <path-to-repo-root>`;
- Create and go to build folder `mkdir -p build-release; cd build-release`;
- Run CMake `cmake .. -G <generator-name> -DCMAKE_PREFIX_PATH=<path-to-qt-installation> -DCMAKE_BUILD_TYPE=Release`;
- Run build. For Ninja generator it looks like `ninja -j<number-of-threads-to-build>`.

## Build with MSVC

- Clone this repository `git clone <url> <path>`;
- Open root folder in IDE;
- Build, possibly specify build configurations and path to Qt library.

## Run and debug

- Since we link with Qt dynamically don't forget to add `<qt-path>/<abi-arch>/bin` and `<qt-path>/<abi-arch>/plugins/platforms` to `PATH` variable.
