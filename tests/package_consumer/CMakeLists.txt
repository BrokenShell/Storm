# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.17)

project(StormPackageConsumer LANGUAGES CXX)

find_package(Storm 5 CONFIG REQUIRED)

function(assert_no_storm_policy_leak target)
    foreach(property IN ITEMS INTERFACE_COMPILE_OPTIONS INTERFACE_LINK_OPTIONS)
        get_target_property(value ${target} ${property})
        if(value AND value MATCHES "(-Werror|/WX|-fsanitize=)")
            message(FATAL_ERROR "${target} leaked ${property} into its consumer: ${value}")
        endif()
    endforeach()
endfunction()

assert_no_storm_policy_leak(Storm::Storm)

add_executable(storm_package_consumer main.cpp)
target_link_libraries(storm_package_consumer PRIVATE Storm::Storm)
set_target_properties(storm_package_consumer PROPERTIES CXX_EXTENSIONS OFF)

enable_testing()
add_test(NAME storm.package_consumer COMMAND storm_package_consumer)
