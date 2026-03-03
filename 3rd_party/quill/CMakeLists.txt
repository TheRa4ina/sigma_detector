# Quill
set(QUILL_STATIC_LIB_NAME quill_static)

add_library(quill_static STATIC
        logging.h
        quill_static.h
        quill_static.cpp)

target_include_directories(${QUILL_STATIC_LIB_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${3RD_PARTY_DIR}>
        $<INSTALL_INTERFACE:${3RD_PARTY_DIR}>)

set_common_compile_options(${QUILL_STATIC_LIB_NAME})

target_link_libraries(${QUILL_STATIC_LIB_NAME} PUBLIC quill::quill)