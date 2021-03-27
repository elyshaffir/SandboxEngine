cmake_minimum_required(VERSION 3.17)
project(SandboxEngine VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SandboxEngine main.cpp)

set(libraries D:/DATA/Documents/C++/libraries)

set(glm_DIR ${libraries}/glm/cmake/glm)
find_package(glm REQUIRED)
target_link_libraries(SandboxEngine glm::glm)

set(WITH_GFLAGS OFF)
set(BUILD_TESTING OFF)
set(WITH_UNWIND OFF)
add_subdirectory(${libraries}/glog glog_build_dir EXCLUDE_FROM_ALL)
target_link_libraries(SandboxEngine glog::glog)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(${libraries}/glfw-3.3.2 glfw_build_dir EXCLUDE_FROM_ALL)
target_link_libraries(SandboxEngine glfw)

find_package(Vulkan)
target_link_libraries(SandboxEngine Vulkan::Vulkan)

target_include_directories(SandboxEngine PRIVATE Include)
file(GLOB_RECURSE sources Source/*.cpp)
target_sources(SandboxEngine PRIVATE ${sources})

add_compile_definitions(ENGINE_NAME="Sandbox")

set(DEBUG true)
if (DEBUG)
    add_compile_definitions(ENABLE_VALIDATION_LAYERS)
endif()
