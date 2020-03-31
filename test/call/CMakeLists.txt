file(GLOB SRC_LIST 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

set(TARGET_NAME "call")
add_executable(${TARGET_NAME} ${SRC_LIST} ${CMAKE_CURRENT_SOURCE_DIR}/../doctest.h)
target_link_libraries(${TARGET_NAME} ${PROJECT_NAME})
set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "test")

add_test(NAME call_test COMMAND $<TARGET_FILE:${TARGET_NAME}>)
