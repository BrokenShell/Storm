// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

auto main() -> int {
    Storm::Generator generator{std::uint64_t{5}};
    Storm::wide_index_selector selector{generator.engine(), 1U};
    return Storm::uniform_index(generator.engine(), 1U) == 0U &&
                   selector(generator.engine()) == 0U
               ? 0
               : 1;
}
