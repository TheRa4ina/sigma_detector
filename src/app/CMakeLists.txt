# front
add_subdirectory(ui)

add_executable(launcher
gui_launcher.cpp
gui_host.cpp
engine_config.cpp
verdict_buffer.cpp
)

target_link_libraries(launcher PRIVATE
imgui
imgui_filebrowser
Threads::Threads
glfw
OpenGL::GL
gui
quill_static
)

target_include_directories(launcher PRIVATE
${SRC_DIR}
)

add_custom_target(copy_fonts ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_LIST_DIR}/fonts"
            "${CMAKE_CURRENT_BINARY_DIR}/fonts"
    COMMENT "Copying fonts folder to build directory"
)

add_dependencies(launcher
copy_fonts
)