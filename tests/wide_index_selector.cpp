// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

class reference_wide_index_selector {
public:
    explicit reference_wide_index_selector(Storm::engine_type& engine, const std::size_t size)
        : permutation_{make_permutation(engine, size)},
          rotation_width_{integer_sqrt(size)},
          distance_{static_cast<double>(rotation_width_) / 4.0} {}

    auto operator()(Storm::engine_type& engine) -> std::size_t {
        distance_type sample = 0;
        do {
            sample = distance_(engine);
        } while (sample >= static_cast<distance_type>(rotation_width_));

        const std::size_t advance =
            (static_cast<std::size_t>(sample) + 1) % permutation_.size();
        const auto middle =
            permutation_.begin() + static_cast<std::ptrdiff_t>(advance);
        std::ranges::rotate(permutation_, middle);
        return permutation_.front();
    }

private:
    using distance_type = long long;

    static auto make_permutation(Storm::engine_type& engine, const std::size_t size)
        -> std::vector<std::size_t> {
        if (size == 0) {
            throw std::invalid_argument{"wide index reference requires a nonzero size"};
        }
        std::vector<std::size_t> permutation(size);
        std::iota(permutation.begin(), permutation.end(), std::size_t{0});
        std::ranges::shuffle(permutation, engine);
        return permutation;
    }

    static constexpr auto integer_sqrt(const std::size_t value) noexcept -> std::size_t {
        std::size_t root = 0;
        while (root + 1 <= value / (root + 1)) {
            ++root;
        }
        return root;
    }

    std::vector<std::size_t> permutation_;
    std::size_t rotation_width_;
    std::poisson_distribution<distance_type> distance_;
};

void test_api_and_invalid_domain() {
    static_assert(std::is_constructible_v<
                  Storm::wide_index_selector, Storm::engine_type&, std::size_t>);
    static_assert(!std::is_constructible_v<Storm::wide_index_selector, std::size_t>);
    static_assert(std::is_same_v<
                  decltype(std::declval<Storm::wide_index_selector&>()(
                      std::declval<Storm::engine_type&>())),
                  std::size_t>);

    Storm::engine_type engine{std::uint64_t{91}};
    const Storm::engine_type initial_state = engine;
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::wide_index_selector(engine, std::size_t{0}));
    STORM_CHECK(engine == initial_state);

    Storm::wide_index_selector size_one{engine, std::size_t{1}};
    const Storm::engine_type before_selection = engine;
    STORM_CHECK(size_one(engine) == 0U);
    STORM_CHECK(engine != before_selection);

    constexpr std::size_t size_max = std::numeric_limits<std::size_t>::max();
    constexpr std::size_t root = Storm::detail::integer_sqrt(size_max);
    static_assert(root <= size_max / root);
    static_assert(root + 1 > size_max / (root + 1));
    static_assert(Storm::detail::add_modulo(size_max - 1, 2, size_max) == 1);
    static_assert(Storm::detail::add_modulo(0, 1, 1) == 0);
}

void test_reference_equivalence() {
    constexpr std::array<std::size_t, 5> sizes{1, 2, 4, 100, 1'000};
    constexpr std::size_t seed_count = 24;
    constexpr std::size_t draws_per_case = 1'000;
    static_assert(sizes.size() * seed_count * draws_per_case == 120'000);

    for (const std::size_t size : sizes) {
        for (std::size_t seed = 0; seed < seed_count; ++seed) {
            Storm::engine_type actual_engine{static_cast<std::uint64_t>(seed)};
            Storm::engine_type reference_engine{static_cast<std::uint64_t>(seed)};
            Storm::wide_index_selector actual{actual_engine, size};
            reference_wide_index_selector reference{reference_engine, size};
            STORM_CHECK(actual_engine == reference_engine);

            for (std::size_t draw = 0; draw < draws_per_case; ++draw) {
                STORM_CHECK(actual(actual_engine) == reference(reference_engine));
                STORM_CHECK(actual_engine == reference_engine);
            }
        }
    }
}

void test_bounds_coverage_and_no_immediate_repeat() {
    constexpr std::array<std::size_t, 5> sizes{1, 2, 4, 100, 1'000};
    for (const std::size_t size : sizes) {
        Storm::engine_type engine{std::uint64_t{0x51EC'7000} +
                                  static_cast<std::uint64_t>(size)};
        Storm::wide_index_selector selector{engine, size};
        std::vector<bool> seen(size);
        std::size_t previous = size;
        const std::size_t draws = size * 200;
        for (std::size_t draw = 0; draw < draws; ++draw) {
            const std::size_t selected = selector(engine);
            STORM_CHECK(selected < size);
            if (size > 1) {
                STORM_CHECK(selected != previous);
            }
            seen[selected] = true;
            previous = selected;
        }
        STORM_CHECK(std::ranges::all_of(seen, [](const bool value) { return value; }));
    }
}

void test_broad_marginal_uniformity() {
    constexpr std::size_t size = 100;
    constexpr std::size_t samples = 500'000;
    constexpr double expected = static_cast<double>(samples) / static_cast<double>(size);
    constexpr double tolerance = expected * 0.10;

    Storm::engine_type engine{std::uint64_t{0xC0FF'EE12'3456'7890}};
    Storm::wide_index_selector selector{engine, size};
    std::array<std::size_t, size> buckets{};
    for (std::size_t draw = 0; draw < samples; ++draw) {
        ++buckets[selector(engine)];
    }
    for (const std::size_t count : buckets) {
        STORM_CHECK(std::fabs(static_cast<double>(count) - expected) <= tolerance);
    }
}

}  // namespace

auto main() -> int {
    test_api_and_invalid_domain();
    test_reference_equivalence();
    test_bounds_coverage_and_no_immediate_repeat();
    test_broad_marginal_uniformity();
    return storm_test::finish();
}
