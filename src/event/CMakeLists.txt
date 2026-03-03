#event
set(LOGSOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/logsources)
add_subdirectory(logsources)

add_executable(bench
speedcheck.cpp
)
add_library(eventTypes INTERFACE
event.h
)
target_include_directories(eventTypes INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)

target_link_libraries(bench PRIVATE
ryml
base64
benchmark::benchmark
)

#target_link_libraries(event_reciever PRIVATE
#pugixml::pugixml
#quill
#)