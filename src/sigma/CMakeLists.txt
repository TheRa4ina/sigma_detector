# sigma

add_subdirectory(rule_parser)

add_library(engine
engine.cpp
)
target_link_libraries(engine PUBLIC
sigmarule
Boost::asio
concurrentqueue
sysmonReceiver
quill_static
)
target_include_directories(engine PUBLIC
${SRC_DIR}
)