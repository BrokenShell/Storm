// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include <string_view>

auto main() -> int {
    constexpr std::string_view expected_version{"5.0.2"};
    return std::string_view{Storm::version} == expected_version ? 0 : 1;
}
