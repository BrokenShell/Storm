# SPDX-License-Identifier: MIT

find_package(Threads REQUIRED)

function(storm_add_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} PRIVATE Storm::Storm)
    storm_configure_owned_target(${target})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

storm_add_test(storm.core_contracts core_contracts.cpp)
storm_add_test(storm.prepared_weighted_index prepared_weighted_index.cpp)
storm_add_test(storm.statistical_smoke statistical_smoke.cpp)
storm_add_test(storm.wide_index_selector wide_index_selector.cpp)
storm_add_test(storm.version version.cpp)
target_compile_definitions(
    storm.version
    PRIVATE STORM_TEST_PROJECT_VERSION="${PROJECT_VERSION}"
)
storm_add_test(storm.self_contained_canonical self_contained_canonical.cpp)
if(STORM_ENABLE_CLANG_TIDY)
    set_target_properties(
        storm.self_contained_canonical
        PROPERTIES
            CXX_CLANG_TIDY "${STORM_CLANG_TIDY_EXECUTABLE};--warnings-as-errors=*"
    )
endif()
storm_add_test(
    storm.multi_tu
    multi_tu_main.cpp
    multi_tu_a.cpp
    multi_tu_b.cpp
)
storm_add_test(storm.thread_local thread_local.cpp)
target_link_libraries(storm.thread_local PRIVATE Threads::Threads)
