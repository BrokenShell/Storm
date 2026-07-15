// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

auto main() -> int {
    Storm::Generator generator{std::uint64_t{5}};
    return Storm::uniform_index(generator.engine(), 1U) == 0U ? 0 : 1;
}
