// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <string_view>

#ifndef STORM_TEST_PROJECT_VERSION
#error "STORM_TEST_PROJECT_VERSION must be supplied by the test build"
#endif

auto main() -> int {
    constexpr std::string_view expected{"5.0.1"};
    static_assert(std::string_view{Storm::version} == expected);
    static_assert(std::string_view{STORM_TEST_PROJECT_VERSION} == expected);
    STORM_CHECK(std::string_view{Storm::version} == expected);
    STORM_CHECK(std::string_view{STORM_TEST_PROJECT_VERSION} == expected);
    return storm_test::finish();
}
