// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <array>
#include <cmath>
#include <cstddef>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace {

void test_uniform_index_frequencies() {
    constexpr std::size_t bucket_count = 8;
    constexpr std::size_t samples = 32'768;
    constexpr double expected = static_cast<double>(samples) / static_cast<double>(bucket_count);
    constexpr double tolerance = expected * 0.15;
    std::array<std::size_t, bucket_count> buckets{};
    Storm::Generator generator{Storm::Unsigned{1'234'567}};
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
    Storm::Generator generator{Storm::Unsigned{7'654'321}};
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
    Storm::Generator generator{Storm::Unsigned{424'242}};
    double total = 0.0;
    for (std::size_t index = 0; index < samples; ++index) {
        total += Storm::canonical(generator.engine());
    }
    STORM_CHECK(storm_test::approximately(total / static_cast<double>(samples), 0.5, 0.02));
}

void test_legacy_linear_shapes() {
    constexpr int samples = 20'000;
    constexpr Storm::Integer size = 101;
    double front_total = 0.0;
    double middle_total = 0.0;
    double back_total = 0.0;
    double negative_front_total = 0.0;
    double negative_middle_total = 0.0;
    double negative_back_total = 0.0;
    Storm::Engine::seed(Storm::Unsigned{314'159});
    for (int index = 0; index < samples; ++index) {
        front_total += static_cast<double>(Storm::GetIndex::front_linear(size));
        middle_total += static_cast<double>(Storm::GetIndex::middle_linear(size));
        back_total += static_cast<double>(Storm::GetIndex::back_linear(size));
        negative_front_total += static_cast<double>(Storm::GetIndex::front_linear(-size));
        negative_middle_total += static_cast<double>(Storm::GetIndex::middle_linear(-size));
        negative_back_total += static_cast<double>(Storm::GetIndex::back_linear(-size));
    }
    const double front_mean = front_total / static_cast<double>(samples);
    const double middle_mean = middle_total / static_cast<double>(samples);
    const double back_mean = back_total / static_cast<double>(samples);
    const double negative_front_mean = negative_front_total / static_cast<double>(samples);
    const double negative_middle_mean = negative_middle_total / static_cast<double>(samples);
    const double negative_back_mean = negative_back_total / static_cast<double>(samples);
    STORM_CHECK(front_mean < 0.42 * static_cast<double>(size));
    STORM_CHECK(middle_mean > 0.42 * static_cast<double>(size));
    STORM_CHECK(middle_mean < 0.58 * static_cast<double>(size));
    STORM_CHECK(back_mean > 0.58 * static_cast<double>(size));
    STORM_CHECK(negative_front_mean < -0.58 * static_cast<double>(size));
    STORM_CHECK(negative_middle_mean > -0.58 * static_cast<double>(size));
    STORM_CHECK(negative_middle_mean < -0.42 * static_cast<double>(size));
    STORM_CHECK(negative_back_mean > -0.42 * static_cast<double>(size));
}

void test_binomial_mean() {
    constexpr int samples = 10'000;
    double total = 0.0;
    Storm::Engine::seed(Storm::Unsigned{271'828});
    for (int index = 0; index < samples; ++index) {
        total += static_cast<double>(Storm::GetInt::binomial_variate(10, 0.3));
    }
    STORM_CHECK(storm_test::approximately(total / static_cast<double>(samples), 3.0, 0.15));
}

}  // namespace

auto main() -> int {
    test_uniform_index_frequencies();
    test_die_frequencies();
    test_canonical_mean();
    test_legacy_linear_shapes();
    test_binomial_mean();
    return storm_test::finish();
}

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
