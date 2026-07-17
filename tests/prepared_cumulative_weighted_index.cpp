// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace {

void test_validation_and_construction_engine_state() {
    Storm::engine_type engine{std::uint64_t{901}};
    const Storm::engine_type initial_state = engine;
    Storm::seed(std::uint64_t{901});
    const Storm::engine_type initial_thread_state = Storm::thread_engine();
    const Storm::PreparedCumulativeWeightedIndex valid{std::array{0.0, 2.0, 2.0, 5.0}};
    const Storm::PreparedCumulativeWeightedIndex integer_boundaries{std::array{1, 3, 6}};
    STORM_CHECK(engine == initial_state);
    STORM_CHECK(Storm::thread_engine() == initial_thread_state);
    STORM_CHECK(integer_boundaries(engine) < 3U);
    STORM_CHECK(Storm::thread_engine() == initial_thread_state);

    STORM_EXPECT_THROWS(
        std::invalid_argument,
        Storm::PreparedCumulativeWeightedIndex(std::initializer_list<double>{}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::PreparedCumulativeWeightedIndex({0.0, 0.0, 0.0}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::PreparedCumulativeWeightedIndex({1.0, -1.0}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::PreparedCumulativeWeightedIndex({1.0, 0.5}));
    STORM_EXPECT_THROWS(
        std::invalid_argument,
        Storm::PreparedCumulativeWeightedIndex(
            {1.0, std::numeric_limits<double>::quiet_NaN()}));
    STORM_EXPECT_THROWS(
        std::invalid_argument,
        Storm::PreparedCumulativeWeightedIndex(
            {1.0, std::numeric_limits<double>::infinity()}));
    STORM_CHECK(Storm::thread_engine() == initial_thread_state);

    static_assert(!noexcept(Storm::PreparedCumulativeWeightedIndex({1.0})));
    static_assert(!noexcept(valid(engine)));
}

void test_relative_and_cumulative_equivalence() {
    const std::array<std::vector<double>, 9> tables{
        std::vector<double>{1.0},
        std::vector<double>{std::numeric_limits<double>::denorm_min()},
        std::vector<double>{1.0, 1.0, 1.0, 1.0},
        std::vector<double>{1.0, 3.0, 2.0, 8.0},
        std::vector<double>{0.0, 2.0, 3.0},
        std::vector<double>{2.0, 0.0, 3.0},
        std::vector<double>{2.0, 3.0, 0.0},
        std::vector<double>{0.0, 1.0, 0.0, 7.0, 0.0, 2.0, 0.0},
        std::vector<double>(1'000, 1.0),
    };
    constexpr std::array<std::uint64_t, 5> seeds{0, 1, 42, 4'294'967'291ULL,
                                                 0xCAFE'F00D'1234'5678ULL};

    for (const auto& weights : tables) {
        std::vector<double> cumulative;
        cumulative.reserve(weights.size());
        double total = 0.0;
        for (const double weight : weights) {
            total += weight;
            cumulative.push_back(total);
        }

        const Storm::PreparedWeightedIndex relative{weights};
        const Storm::PreparedCumulativeWeightedIndex prepared_cumulative{cumulative};
        for (const std::uint64_t seed : seeds) {
            Storm::engine_type relative_engine{seed};
            Storm::engine_type cumulative_engine{seed};
            for (std::size_t draw = 0; draw < 20'000; ++draw) {
                STORM_CHECK(relative(relative_engine) ==
                            prepared_cumulative(cumulative_engine));
                STORM_CHECK(relative_engine == cumulative_engine);
            }
        }
    }
}

void test_supplied_strict_boundary_is_preserved() {
    constexpr std::uint64_t seed = 317;
    Storm::engine_type draw_engine{seed};
    std::uniform_real_distribution<double> distribution{0.0, 1.0};
    const double boundary = distribution(draw_engine);
    STORM_CHECK(boundary > 0.0);
    STORM_CHECK(boundary < 1.0);

    const Storm::PreparedCumulativeWeightedIndex prepared{{boundary, 1.0}};
    Storm::engine_type engine{seed};
    STORM_CHECK(prepared(engine) == 1U);
    STORM_CHECK(engine == draw_engine);
}

void test_subnormal_endpoint_correction() {
    constexpr double boundary = std::numeric_limits<double>::denorm_min();
    const Storm::PreparedCumulativeWeightedIndex single{
        std::initializer_list<double>{boundary}};
    Storm::engine_type single_engine{std::uint64_t{0}};
    STORM_CHECK(single(single_engine) == 0U);
    STORM_CHECK(single(single_engine) == 0U);

    const Storm::PreparedCumulativeWeightedIndex duplicates{
        {0.0, boundary, boundary, boundary}};
    Storm::engine_type duplicate_engine{std::uint64_t{0}};
    for (std::size_t draw = 0; draw < 10'000; ++draw) {
        STORM_CHECK(duplicates(duplicate_engine) == 1U);
    }
}

void test_duplicate_boundaries_are_never_selected() {
    const Storm::PreparedCumulativeWeightedIndex prepared{{0.0, 1.0, 1.0, 5.0, 5.0}};
    Storm::engine_type engine{std::uint64_t{808}};
    for (std::size_t draw = 0; draw < 100'000; ++draw) {
        const std::size_t selected = prepared(engine);
        STORM_CHECK(selected == 1U || selected == 3U);
    }
}

}  // namespace

auto main() -> int {
    test_validation_and_construction_engine_state();
    test_relative_and_cumulative_equivalence();
    test_supplied_strict_boundary_is_preserved();
    test_subnormal_endpoint_correction();
    test_duplicate_boundaries_are_never_selected();
    return storm_test::finish();
}
