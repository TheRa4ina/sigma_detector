add_library(sysmonReceiver
	sysmon_normalizer.cpp
	sysmon_receiver.cpp
)

target_include_directories(sysmonReceiver PRIVATE
${SRC_DIR}
)

target_link_libraries(sysmonReceiver PRIVATE
pugixml::pugixml
concurrentqueue
quill_static
)

add_subdirectory(tests)