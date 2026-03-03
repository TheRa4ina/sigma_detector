#rule_parser
add_library(sigmarule
selection.cpp
modifier.cpp
condition.cpp
rule.cpp
rule_parser.cpp
utilities.cpp
mod_utils.cpp
encoding_utils.cpp
logsource.cpp
selector_factory.cpp
)

target_include_directories(sigmarule PUBLIC
${SIGMA_DIR}
${SRC_DIR}
)

target_link_libraries(sigmarule PUBLIC
ryml
base64
)

add_subdirectory(tests)