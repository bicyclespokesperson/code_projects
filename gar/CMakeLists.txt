# Based on: 
# http://derekmolloy.ie/hello-world-introductions-to-cmake/
# https://blog.conan.io/2018/06/11/Transparent-CMake-Integration.html
#
# Set environment variable VULKAN_DIR to the sdk directory containing the bin folder

# Build commands (from root directory):
# $ conan install .
# $ cmake CMakeLists.txt
# $ make

cmake_minimum_required(VERSION 3.10)

project (rasterizer)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP(TARGETS)

# Place the complied executable in the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(vulkan REQUIRED)

add_executable(rasterizer ${SOURCES})
target_link_libraries(rasterizer CONAN_PKG::Assimp)
target_link_libraries(rasterizer CONAN_PKG::glfw)
target_link_libraries(rasterizer CONAN_PKG::glm)
target_link_libraries(rasterizer CONAN_PKG::range-v3)
target_link_libraries(rasterizer CONAN_PKG::stb)
target_link_libraries(rasterizer Vulkan::Vulkan)

set_target_properties(rasterizer PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )
if(MSVC)
  target_compile_options(rasterizer PRIVATE /W4 /WX)
else()
  target_compile_options(rasterizer PRIVATE -Wall -Wextra -pedantic -Wno-missing-braces)
endif()



# Compile shaders from glsl to spir-v
# From: https://gist.github.com/vlsh/a0d191701cb48f157b05be7f74d79396
# set(GLSL_VALIDATOR "$ENV{VULKAN_DIR}/Bin/glslangValidator.exe") # Windows Version
set(GLSL_VALIDATOR "$ENV{VULKAN_DIR}/bin/glslangValidator")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "shaders/*.frag"
    "shaders/*.vert"
    )

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
    )

add_dependencies(rasterizer Shaders)

add_custom_command(TARGET rasterizer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:rasterizer>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:rasterizer>/shaders"
        )

