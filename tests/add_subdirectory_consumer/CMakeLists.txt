# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.17)

project(StormAddSubdirectoryConsumer LANGUAGES CXX)

set(
    STORM_SOURCE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    CACHE PATH
    "Path to the Storm source tree"
)

set(STORM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(STORM_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(STORM_WARNINGS_AS_ERRORS ON CACHE BOOL "" FORCE)
set(STORM_ENABLE_ASAN_UBSAN OFF CACHE BOOL "" FORCE)
set(STORM_ENABLE_TSAN OFF CACHE BOOL "" FORCE)

add_subdirectory("${STORM_SOURCE_DIR}" storm)

function(assert_no_storm_policy_leak target)
    foreach(property IN ITEMS INTERFACE_COMPILE_OPTIONS INTERFACE_LINK_OPTIONS)
        get_target_property(value ${target} ${property})
        if(value AND value MATCHES "(-Werror|/WX|-fsanitize=)")
            message(FATAL_ERROR "${target} leaked ${property} into its consumer: ${value}")
        endif()
    endforeach()
endfunction()

assert_no_storm_policy_leak(Storm::Storm)

add_executable(storm_add_subdirectory_consumer main.cpp)
target_link_libraries(storm_add_subdirectory_consumer PRIVATE Storm::Storm)
set_target_properties(storm_add_subdirectory_consumer PROPERTIES CXX_EXTENSIONS OFF)

enable_testing()
add_test(NAME storm.add_subdirectory_consumer COMMAND storm_add_subdirectory_consumer)
