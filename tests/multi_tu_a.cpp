// SPDX-License-Identifier: MIT
#include "multi_tu_shared.hpp"

auto multi_tu_seed_and_draw() -> std::int64_t {
    Storm::seed(std::uint64_t{73});
    return Storm::uniform_integer(-10'000, 10'000);
}
