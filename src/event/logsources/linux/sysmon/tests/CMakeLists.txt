# sysmon normalizer tests
add_executable(normalizerTests
normalizer_tests.cpp
)

target_link_libraries(normalizerTests PRIVATE
GTest::gtest_main
sysmonReceiver
)

target_include_directories(normalizerTests PRIVATE
${LOGSOURCES_DIR}
)

add_custom_target(testData ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_LIST_DIR}/res
	${CMAKE_CURRENT_BINARY_DIR}/res
)

add_dependencies(normalizerTests
testData
)

gtest_discover_tests(normalizerTests)