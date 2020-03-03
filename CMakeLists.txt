cmake_minimum_required(VERSION 3.1)

project(klua)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "postfix for Debug-built libraries" FORCE)
set(CMAKE_RELWITHDEBINFO_POSTFIX "_rwdi" CACHE STRING "postfix for MinsizeRelease-built libraries" FORCE)
set(CMAKE_MINSIZEREL_POSTFIX "_msr" CACHE STRING "postfix for ReleaseWithDebug-built libraries" FORCE)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if (WIN32)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

if (MSVC)
    add_definitions("/wd4100")
    add_definitions("/W4")
endif()

enable_testing()

#######################################################################################

function (kcompile_as_cpp)
    foreach(filename ${ARGN})
        if (filename MATCHES "^.*\.c$")
            set_source_files_properties(${filename} PROPERTIES LANGUAGE CXX)
            if (MSVC)
                set_source_files_properties(${filename} PROPERTIES COMPILE_FLAGS "/W3 /wd4334")
            endif()
        endif(filename MATCHES "^.*\.c$")
    endforeach(filename)
endfunction()

file(GLOB LUA_SRC_LIST
    "${PROJECT_SOURCE_DIR}/include/k/*.h"
    "${PROJECT_SOURCE_DIR}/include/k/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/k/*.c"
    )
list(REMOVE_ITEM LUA_SRC_LIST "${PROJECT_SOURCE_DIR}/src/k/lua.c")
list(REMOVE_ITEM LUA_SRC_LIST "${PROJECT_SOURCE_DIR}/src/k/luac.c")

file(GLOB SRC_LIST
    "${PROJECT_SOURCE_DIR}/include/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    )

kcompile_as_cpp(${LUA_SRC_LIST})
source_group("lua" FILES ${LUA_SRC_LIST})

add_library(${PROJECT_NAME} ${LUA_SRC_LIST} ${SRC_LIST})
target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/include/k
    )

if (NOT WIN32)
    add_definitions(-DLUA_USE_LINUX)
    target_link_libraries(${PROJECT_NAME}
        m
        dl
        )
endif()


install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION "include"
    )
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )


add_subdirectory(test)
add_subdirectory(examples)
