// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace {

class LinearWeightedIndex {
public:
    explicit LinearWeightedIndex(const std::vector<double>& weights) {
        double total = 0.0;
        for (const double weight : weights) {
            total += weight;
            cumulative_.push_back(total);
        }
        total_ = total;
    }

    auto operator()(Storm::engine_type& engine) const -> std::size_t {
        std::uniform_real_distribution<double> distribution{0.0, total_};
        const double draw = distribution(engine);
        const auto selected = std::find_if(cumulative_.begin(), cumulative_.end(),
                                           [draw](const double boundary) {
                                               return boundary > draw;
                                           });
        return static_cast<std::size_t>(selected - cumulative_.begin());
    }

private:
    std::vector<double> cumulative_;
    double total_{0.0};
};

void test_validation_and_construction_engine_state() {
    Storm::engine_type engine{std::uint64_t{901}};
    const Storm::engine_type initial_state = engine;
    Storm::seed(std::uint64_t{901});
    const Storm::engine_type initial_thread_state = Storm::thread_engine();
    const Storm::PreparedWeightedIndex valid{std::array{0.0, 2.0, 3.0}};
    const Storm::PreparedWeightedIndex integer_weights{std::array{1, 2, 3}};
    STORM_CHECK(engine == initial_state);
    STORM_CHECK(Storm::thread_engine() == initial_thread_state);
    STORM_CHECK(integer_weights(engine) < 3U);
    STORM_CHECK(Storm::thread_engine() == initial_thread_state);

    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::PreparedWeightedIndex(std::initializer_list<double>{}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::PreparedWeightedIndex({0.0, 0.0, 0.0}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::PreparedWeightedIndex({1.0, -1.0}));
    STORM_EXPECT_THROWS(
        std::invalid_argument,
        Storm::PreparedWeightedIndex({1.0, std::numeric_limits<double>::quiet_NaN()}));
    STORM_EXPECT_THROWS(
        std::invalid_argument,
        Storm::PreparedWeightedIndex({1.0, std::numeric_limits<double>::infinity()}));
    STORM_EXPECT_THROWS(
        std::overflow_error,
        Storm::PreparedWeightedIndex(
            {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()}));
    STORM_CHECK(Storm::thread_engine() == initial_thread_state);

    static_assert(!noexcept(Storm::PreparedWeightedIndex({1.0})));
    static_assert(!noexcept(valid(engine)));
}

void test_strict_cumulative_boundary() {
    constexpr std::uint64_t seed = 317;
    Storm::engine_type draw_engine{seed};
    std::uniform_real_distribution<double> distribution{0.0, 1.0};
    const double boundary = distribution(draw_engine);
    STORM_CHECK(boundary > 0.0);
    STORM_CHECK(boundary < 1.0);

    const Storm::PreparedWeightedIndex prepared{{boundary, 1.0 - boundary}};
    Storm::engine_type engine{seed};
    STORM_CHECK(prepared(engine) == 1U);
    STORM_CHECK(engine == draw_engine);
}

void test_reference_equivalence() {
    const std::array<std::vector<double>, 8> tables{
        std::vector<double>{1.0},
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
        const Storm::PreparedWeightedIndex prepared{weights};
        const LinearWeightedIndex reference{weights};
        for (const std::uint64_t seed : seeds) {
            Storm::engine_type actual_engine{seed};
            Storm::engine_type reference_engine{seed};
            for (std::size_t draw = 0; draw < 20'000; ++draw) {
                STORM_CHECK(prepared(actual_engine) == reference(reference_engine));
                STORM_CHECK(actual_engine == reference_engine);
            }
        }
    }
}

void test_zero_weights_are_never_selected() {
    const Storm::PreparedWeightedIndex prepared{{0.0, 1.0, 0.0, 4.0, 0.0}};
    Storm::engine_type engine{std::uint64_t{808}};
    for (std::size_t draw = 0; draw < 100'000; ++draw) {
        const std::size_t selected = prepared(engine);
        STORM_CHECK(selected == 1U || selected == 3U);
    }
}

}  // namespace

auto main() -> int {
    test_validation_and_construction_engine_state();
    test_strict_cumulative_boundary();
    test_reference_equivalence();
    test_zero_weights_are_never_selected();
    return storm_test::finish();
}
