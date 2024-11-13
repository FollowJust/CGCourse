# Adding some hardcoded thirdparty libs
add_subdirectory(GSL)
include_directories(GSL/include)

add_subdirectory(glm)
include_directories(glm)

add_subdirectory(tinygltf)
file(CREATE_LINK "${CMAKE_CURRENT_LIST_DIR}/tinygltf/json.hpp" "${CMAKE_CURRENT_LIST_DIR}/tinygltf/tinygltf/json.hpp" COPY_ON_ERROR)
file(CREATE_LINK "${CMAKE_CURRENT_LIST_DIR}/tinygltf/stb_image.h" "${CMAKE_CURRENT_LIST_DIR}/tinygltf/tinygltf/stb_image.h" COPY_ON_ERROR)
file(CREATE_LINK "${CMAKE_CURRENT_LIST_DIR}/tinygltf/stb_image_write.h" "${CMAKE_CURRENT_LIST_DIR}/tinygltf/tinygltf/stb_image_write.h" COPY_ON_ERROR)
file(CREATE_LINK "${CMAKE_CURRENT_LIST_DIR}/tinygltf/tiny_gltf.h" "${CMAKE_CURRENT_LIST_DIR}/tinygltf/tinygltf/tiny_gltf.h" COPY_ON_ERROR)
include_directories(tinygltf)

# Disable warnings from thirdparty libs
if (MSVC)
    target_compile_options(GSL INTERFACE /WX-)
    target_compile_options(glm INTERFACE /WX-)
    target_compile_options(tinygltf INTERFACE /WX-)
else()
    target_compile_options(GSL INTERFACE -Wno-error)
    target_compile_options(glm INTERFACE -Wno-error)
    target_compile_options(tinygltf INTERFACE -Wno-error)
endif()


add_library(thirdparty::GSL ALIAS GSL)
add_library(thirdparty::glm ALIAS glm)
add_library(thirdparty::tinygltf ALIAS tinygltf)
