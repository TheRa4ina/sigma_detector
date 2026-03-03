#3rd_party/imgui
add_library(imgui
${imgui_SOURCE_DIR}/imgui.cpp
${imgui_SOURCE_DIR}/imgui_demo.cpp
${imgui_SOURCE_DIR}/imgui_draw.cpp
${imgui_SOURCE_DIR}/imgui_tables.cpp
${imgui_SOURCE_DIR}/imgui_widgets.cpp
${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_library(imgui_filebrowser INTERFACE)
target_include_directories(imgui_filebrowser INTERFACE 
    ${imgui_filebrowser_SOURCE_DIR}
)

target_include_directories(imgui PUBLIC
${imgui_SOURCE_DIR}
${imgui_SOURCE_DIR}/backends
${CMAKE_CURRENT_LIST_DIR}/..)

target_link_libraries(imgui OpenGL::GL glfw)