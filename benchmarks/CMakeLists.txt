# SPDX-License-Identifier: MIT

add_executable(storm_benchmark benchmark.cpp)
target_link_libraries(storm_benchmark PRIVATE Storm::Storm)
storm_configure_owned_target(storm_benchmark)
