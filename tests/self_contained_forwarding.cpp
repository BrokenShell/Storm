// SPDX-License-Identifier: MIT
#include <Storm.hpp>

auto main() -> int {
    Storm::Generator generator{Storm::Unsigned{5}};
    return Storm::uniform_index(generator.engine(), 1U) == 0U ? 0 : 1;
}
