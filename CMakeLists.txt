# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.17)

project(Storm VERSION 5.0.0 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(STORM_IS_TOP_LEVEL ON)
else()
    set(STORM_IS_TOP_LEVEL OFF)
endif()

option(STORM_BUILD_TESTS "Build Storm's dependency-free test suite" ${STORM_IS_TOP_LEVEL})
option(STORM_BUILD_BENCHMARKS "Build Storm's opt-in benchmark executable" OFF)
option(STORM_WARNINGS_AS_ERRORS "Treat warnings as errors for Storm-owned targets" OFF)
option(STORM_ENABLE_ASAN_UBSAN "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(STORM_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(STORM_ENABLE_ASAN_UBSAN AND STORM_ENABLE_TSAN)
    message(FATAL_ERROR "ASan/UBSan and TSan cannot be enabled in the same build")
endif()

if((STORM_ENABLE_ASAN_UBSAN OR STORM_ENABLE_TSAN) AND
   NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "Storm's sanitizer options require GCC, Clang, or AppleClang")
endif()

add_library(Storm INTERFACE)
add_library(Storm::Storm ALIAS Storm)

target_compile_features(Storm INTERFACE cxx_std_20)
target_include_directories(
    Storm
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Warning and sanitizer settings belong only to repository-owned executables.
# Keeping them off the exported INTERFACE target prevents policy leakage into
# add_subdirectory and find_package consumers.
function(storm_configure_owned_target target)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
        if(STORM_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(
            ${target}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wconversion
                -Wsign-conversion
                -Wshadow
                -Wundef
        )
        if(STORM_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    else()
        message(WARNING "No strict-warning profile is defined for ${CMAKE_CXX_COMPILER_ID}")
    endif()

    if(STORM_ENABLE_ASAN_UBSAN)
        target_compile_options(
            ${target}
            PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
                -fno-sanitize-recover=all
        )
        target_link_options(
            ${target}
            PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
                -fno-sanitize-recover=all
        )
    elseif(STORM_ENABLE_TSAN)
        target_compile_options(${target} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
        target_link_options(${target} PRIVATE -fsanitize=thread -fno-omit-frame-pointer)
    endif()
endfunction()

if(STORM_BUILD_TESTS)
    enable_testing()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
        message(FATAL_ERROR "STORM_BUILD_TESTS is ON, but tests/CMakeLists.txt is missing")
    endif()
    add_subdirectory(tests)
endif()

if(STORM_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

set(STORM_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/Storm")

install(TARGETS Storm EXPORT StormTargets)
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(FILES LICENSE DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/Storm")
install(
    EXPORT StormTargets
    FILE StormTargets.cmake
    NAMESPACE Storm::
    DESTINATION "${STORM_INSTALL_CMAKEDIR}"
)

configure_package_config_file(
    cmake/StormConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/StormConfig.cmake"
    INSTALL_DESTINATION "${STORM_INSTALL_CMAKEDIR}"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/StormConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/StormConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/StormConfigVersion.cmake"
    DESTINATION "${STORM_INSTALL_CMAKEDIR}"
)
