add_library(lib)

target_include_directories(lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/extern/stb)

target_sources(lib PRIVATE lib.cpp)

target_link_directories(lib PRIVATE OpenGL::GL)
