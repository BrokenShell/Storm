// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <array>
#include <cmath>
#include <cstddef>

namespace {

void test_uniform_index_frequencies() {
    constexpr std::size_t bucket_count = 8;
    constexpr std::size_t samples = 32'768;
    constexpr double expected = static_cast<double>(samples) / static_cast<double>(bucket_count);
    constexpr double tolerance = expected * 0.15;
    std::array<std::size_t, bucket_count> buckets{};
    Storm::Generator generator{std::uint64_t{1'234'567}};
    for (std::size_t index = 0; index < samples; ++index) {
        ++buckets[Storm::uniform_index(generator.engine(), bucket_count)];
    }
    for (const auto count : buckets) {
        STORM_CHECK(std::fabs(static_cast<double>(count) - expected) <= tolerance);
    }
}

void test_die_frequencies() {
    constexpr std::size_t samples = 24'000;
    constexpr double expected = static_cast<double>(samples) / 6.0;
    constexpr double tolerance = expected * 0.12;
    std::array<std::size_t, 6> buckets{};
    Storm::Generator generator{std::uint64_t{7'654'321}};
    for (std::size_t index = 0; index < samples; ++index) {
        const auto result = Storm::roll_die(generator.engine(), std::size_t{6});
        ++buckets[static_cast<std::size_t>(result - 1)];
    }
    for (const auto count : buckets) {
        STORM_CHECK(std::fabs(static_cast<double>(count) - expected) <= tolerance);
    }
}

void test_canonical_mean() {
    constexpr std::size_t samples = 20'000;
    Storm::Generator generator{std::uint64_t{424'242}};
    double total = 0.0;
    for (std::size_t index = 0; index < samples; ++index) {
        total += Storm::canonical(generator.engine());
    }
    STORM_CHECK(storm_test::approximately(total / static_cast<double>(samples), 0.5, 0.02));
}

void test_prepared_weighted_index_frequencies() {
    constexpr std::size_t samples = 100'000;
    constexpr std::array<double, 4> expected_fractions{0.1, 0.3, 0.0, 0.6};
    constexpr double tolerance = 0.015;
    std::array<std::size_t, expected_fractions.size()> buckets{};
    const Storm::PreparedWeightedIndex prepared{{1.0, 3.0, 0.0, 6.0}};
    Storm::Generator generator{std::uint64_t{9'876'543}};
    for (std::size_t index = 0; index < samples; ++index) {
        ++buckets[prepared(generator.engine())];
    }
    for (std::size_t index = 0; index < buckets.size(); ++index) {
        const double actual =
            static_cast<double>(buckets[index]) / static_cast<double>(samples);
        STORM_CHECK(std::fabs(actual - expected_fractions[index]) <= tolerance);
    }
}

}  // namespace

auto main() -> int {
    test_uniform_index_frequencies();
    test_die_frequencies();
    test_canonical_mean();
    test_prepared_weighted_index_frequencies();
    return storm_test::finish();
}
