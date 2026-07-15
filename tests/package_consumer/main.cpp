// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include <string_view>

auto forwarding_header_version() -> std::string_view;

auto main() -> int {
    constexpr std::string_view expected_version{"5.0.0"};
    const auto canonical_version = std::string_view{Storm::get_version()};
    return canonical_version == expected_version && forwarding_header_version() == expected_version ? 0 : 1;
}
