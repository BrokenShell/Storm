// SPDX-License-Identifier: MIT
#include <Storm.hpp>

#include <string_view>

auto forwarding_header_version() -> std::string_view {
    return std::string_view{Storm::get_version()};
}
