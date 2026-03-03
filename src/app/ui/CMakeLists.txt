add_library(gui 
main_window.cpp
rule_listbox.cpp
verdict_listbox.cpp
)

target_include_directories(gui PRIVATE
${SRC_DIR}
)

target_link_libraries(gui PUBLIC
imgui
engine
)
target_link_libraries(gui PRIVATE
imgui_filebrowser
)