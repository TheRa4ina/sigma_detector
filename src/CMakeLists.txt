#src
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})
set(SIGMA_DIR ${CMAKE_CURRENT_LIST_DIR}/sigma)
set(EVENT_DIR ${CMAKE_CURRENT_LIST_DIR}/event)
add_subdirectory(event)
add_subdirectory(app)
add_subdirectory(sigma)
