cmake_minimum_required(VERSION 3.20)

project(anitk VERSION 0.0.1
              DESCRIPTION "An animation toolkit"
              LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
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

# nativefiledialog-extended
add_subdirectory(extern/nativefiledialog-extended)

# Configure
find_package(Git)
if (Git_FOUND)
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COMMIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" log -1 --format=%ad --date=local
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COMMIT_DATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
configure_file(config.h.in config.h)


# libanitk lib
add_library(lib)

target_include_directories(lib PUBLIC
    extern/stb)

target_sources(lib PRIVATE lib.cpp crc32.cpp)

target_link_libraries(lib PRIVATE OpenGL::GL)


# anitk
add_executable(anitk)

target_include_directories(anitk PRIVATE
    extern/imgui
    extern/imgui/backends
    extern/imgui/misc/cpp)
target_include_directories(anitk PRIVATE
    extern/nativefiledialog-extended/src/include
)
target_include_directories(anitk PRIVATE
    ${PROJECT_BINARY_DIR})

target_sources(anitk PRIVATE anitk.cpp)
target_sources(anitk PRIVATE
    extern/imgui/imgui.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_widgets.cpp
    # extern/imgui/imgui_demo.cpp
    )
target_sources(anitk PRIVATE
    extern/imgui/backends/imgui_impl_glfw.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp)
target_sources(anitk PRIVATE
    extern/imgui/misc/cpp/imgui_stdlib.cpp)

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_sources(anitk PRIVATE icon.rc)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(anitk PRIVATE -mwindows)
        target_link_libraries(anitk PRIVATE -static gcc stdc++)
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    endif()
endif()

target_link_libraries(anitk PRIVATE OpenGL::GL glfw lib nfd)

# Testing - Only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ANITK_BUILD_TESTING)
        AND BUILD_TESTING AND (NOT CMAKE_BUILD_TYPE STREQUAL "Release"))
  # Use system Catch2 or extern
  # find_package(Catch2 QUIET)
  if (NOT Catch2_FOUND)
      message("Catch2 not installed, building from extern")
      set(CATCH_USE_VALGRIND OFF CACHE BOOL "" FORCE)
      set(CATCH_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
      set(CATCH_ENABLE_COVERAGE OFF CACHE BOOL "" FORCE)
      set(CATCH_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
      add_subdirectory(extern/Catch2)
  endif()
  add_subdirectory(tests)
endif()
