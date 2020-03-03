file(GLOB SRC_LIST
    "${CMAKE_SOURCE_DIR}/include/k/*.h"
    "${CMAKE_SOURCE_DIR}/include/k/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/k/*.c"
    "${CMAKE_SOURCE_DIR}/src/k/*.cc"
    "${CMAKE_SOURCE_DIR}/src/k/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/k/*.cxx"
    )
list(REMOVE_ITEM SRC_LIST "${CMAKE_SOURCE_DIR}/src/k/luac.c")

kcompile_as_cpp(${SRC_LIST})
set(TARGET_NAME "lua")

add_executable(${TARGET_NAME} ${SRC_LIST})
target_include_directories(${TARGET_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/include/k
    )

if (MSVC)
    add_definitions("/W3")
endif()

if (NOT WIN32)
    target_link_libraries(${TARGET_NAME}
        readline
        dl
        m
        )
endif()
