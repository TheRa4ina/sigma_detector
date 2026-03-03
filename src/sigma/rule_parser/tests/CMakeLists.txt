# sysmon normalizer tests
# actually it need better names i think
add_executable(ruleParserTests
utilities_tests.cpp
selector_tests.cpp
)

target_link_libraries(ruleParserTests PRIVATE
GTest::gtest_main
GTest::gmock_main
sigmarule
)

target_include_directories(ruleParserTests PRIVATE
${SRC_DIR}
)

gtest_discover_tests(ruleParserTests)