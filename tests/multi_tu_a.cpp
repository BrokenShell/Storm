// SPDX-License-Identifier: MIT
#include "multi_tu_shared.hpp"

auto multi_tu_seed_and_draw() -> Storm::Integer {
    Storm::seed(Storm::Unsigned{73});
    return Storm::uniform_integer(-10'000, 10'000);
}
