cmake_minimum_required(VERSION 3.20)

project(anitk VERSION 1.0
              DESCRIPTION "An animation toolkit"
              LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use")
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  include(CTest)
endif()

# OpenGL
find_package(OpenGL REQUIRED)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)

# libanitk lib
add_library(lib)

target_include_directories(lib PUBLIC
    include
    extern/stb)

target_sources(lib PRIVATE lib.cpp)

target_link_libraries(lib PRIVATE OpenGL::GL)


# anitk
add_executable(anitk)

target_include_directories(anitk PRIVATE
    extern/imgui
    extern/imgui/backends)

target_sources(anitk PRIVATE anitk.cpp)
target_sources(anitk PRIVATE
    extern/imgui/imgui.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_widgets.cpp
    extern/imgui/imgui_demo.cpp)
target_sources(anitk PRIVATE
    extern/imgui/backends/imgui_impl_glfw.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp)

target_link_libraries(anitk PRIVATE OpenGL::GL glfw lib)


# Testing - Only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
    AND BUILD_TESTING)
  add_subdirectory(extern/Catch2)
  add_subdirectory(tests)
endif()
