// SPDX-License-Identifier: MIT
#include "multi_tu_shared.hpp"

#include "test_harness.hpp"

auto main() -> int {
    Storm::Generator control{Storm::Unsigned{73}};
    const auto expected_first = Storm::uniform_integer(control.engine(), -10'000, 10'000);
    const auto expected_second = Storm::uniform_integer(control.engine(), -10'000, 10'000);
    STORM_CHECK(multi_tu_seed_and_draw() == expected_first);
    STORM_CHECK(multi_tu_draw_again() == expected_second);
    return storm_test::finish();
}
