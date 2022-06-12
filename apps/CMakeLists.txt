add_executable(anitk)

target_include_directories(anitk PRIVATE
    ${PROJECT_SOURCE_DIR}/extern/imgui
    ${PROJECT_SOURCE_DIR}/extern/imgui/backends)

target_sources(anitk PRIVATE anitk.cpp)
target_sources(anitk PRIVATE
    ${PROJECT_SOURCE_DIR}/extern/imgui/imgui.cpp 
    ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_tables.cpp 
    ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_draw.cpp 
    ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_demo.cpp)
target_sources(anitk PRIVATE 
    ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp 
    ${PROJECT_SOURCE_DIR}/extern/imgui/backends/imgui_impl_opengl3.cpp)

target_link_libraries(anitk PRIVATE OpenGL::GL glfw lib)
