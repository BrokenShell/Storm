// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include <string_view>

auto main() -> int {
    constexpr std::string_view expected_version{"5.0.2"};
    Storm::Generator relative_generator{42};
    Storm::Generator cumulative_generator{42};
    const Storm::PreparedWeightedIndex relative{{1.0, 0.0, 3.0}};
    const Storm::PreparedCumulativeWeightedIndex cumulative{{1.0, 1.0, 4.0}};
    const bool version_matches = std::string_view{Storm::version} == expected_version;
    const bool selectors_match = relative(relative_generator.engine()) ==
                                 cumulative(cumulative_generator.engine());
    return version_matches && selectors_match ? 0 : 1;
}
