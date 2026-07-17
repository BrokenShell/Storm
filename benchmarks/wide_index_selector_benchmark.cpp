// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace {

constexpr std::uint64_t seed = 0x71F5'2026'0716'0009ULL;
constexpr std::size_t population_size = 100;
constexpr std::size_t default_iterations = 2'000'000;

class fortuna_6_0_2_compositional_reference {
public:
    explicit fortuna_6_0_2_compositional_reference(Storm::engine_type& engine,
                                                   const std::size_t size)
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
            permutation_.end() - static_cast<std::ptrdiff_t>(advance);
        std::ranges::rotate(permutation_, middle);
        return permutation_.back();
    }

private:
    using distance_type = std::uint64_t;

    static auto make_permutation(Storm::engine_type& engine, const std::size_t size)
        -> std::vector<std::size_t> {
        std::vector<std::size_t> permutation(size);
        std::iota(permutation.begin(), permutation.end(), std::size_t{0});
        const std::size_t last = size - 1;
        std::size_t position = last;
        while (position > 0) {
            --position;
            const auto other = static_cast<std::size_t>(Storm::uniform_unsigned(
                engine,
                static_cast<std::uint64_t>(position),
                static_cast<std::uint64_t>(last)));
            std::swap(permutation[position], permutation[other]);
        }
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

auto mix(const std::uint64_t value) -> std::uint64_t {
    auto mixed = value + 0x9E37'79B9'7F4A'7C15ULL;
    mixed = (mixed ^ (mixed >> 30U)) * 0xBF58'476D'1CE4'E5B9ULL;
    mixed = (mixed ^ (mixed >> 27U)) * 0x94D0'49BB'1331'11EBULL;
    return mixed ^ (mixed >> 31U);
}

template<typename Callable>
auto consume(const std::size_t iterations, Callable& callable) -> std::uint64_t {
    std::uint64_t checksum = 0;
    for (std::size_t index = 0; index < iterations; ++index) {
        checksum ^= mix(static_cast<std::uint64_t>(callable()) +
                        static_cast<std::uint64_t>(index));
    }
    return checksum;
}

template<typename Callable>
auto run_case(const std::string_view label,
              const std::string_view unit,
              const std::size_t iterations,
              Callable& callable) -> std::uint64_t {
    const auto start = std::chrono::steady_clock::now();
    const std::uint64_t checksum = consume(iterations, callable);
    const auto stop = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration<double, std::nano>{stop - start}.count();

    std::cout << std::left << std::setw(42) << label << std::right << std::fixed
              << std::setprecision(2) << std::setw(12)
              << elapsed / static_cast<double>(iterations) << " ns/" << unit
              << "  checksum=" << checksum << '\n';
    return checksum;
}

auto parse_iterations(const int argc, char* argv[]) -> std::size_t {
    if (argc == 1) {
        return default_iterations;
    }
    if (argc != 2) {
        return 0;
    }

    const char* const first = argv[1];
    const char* const last = first + std::strlen(first);
    std::uint64_t parsed = 0;
    const auto [end, error] = std::from_chars(first, last, parsed);
    if (error != std::errc{} || end != last || parsed == 0 ||
        parsed > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        return 0;
    }
    return static_cast<std::size_t>(parsed);
}

}  // namespace

auto main(const int argc, char* argv[]) -> int {
    const std::size_t iterations = parse_iterations(argc, argv);
    if (iterations == 0) {
        std::cerr << "usage: storm_wide_index_benchmark [positive-iteration-count]\n";
        return 2;
    }

    const std::size_t warmup_iterations = std::max<std::size_t>(
        1, std::min<std::size_t>(iterations / 10, 50'000));
    const std::size_t construction_iterations = std::max<std::size_t>(
        1, std::min<std::size_t>(iterations / 100, 100'000));
    const std::size_t construction_warmup = std::max<std::size_t>(
        1, std::min<std::size_t>(construction_iterations / 10, 1'000));

    std::cout << "Storm " << Storm::version << " wide-index selector benchmark\n"
              << "iterations=" << iterations << ", construction_iterations="
              << construction_iterations << ", seed=" << seed
              << ", population=" << population_size << ", warmup=" << warmup_iterations
              << ", construction_warmup=" << construction_warmup
              << "\nReference physically applies Fortuna 6.0.2 positive rotation and data[-1].\n"
              << "Selector preserves that schedule with a subtracting cursor.\n"
              << "Construction cases include construction plus the first selection.\n"
              << "Results are descriptive; no timing threshold is applied.\n\n";

    Storm::engine_type reference_engine{seed};
    Storm::engine_type selector_engine{seed};
    fortuna_6_0_2_compositional_reference reference{
        reference_engine, population_size};
    Storm::wide_index_selector selector{selector_engine, population_size};
    if (reference_engine != selector_engine) {
        std::cerr << "construction engine states differ\n";
        return 1;
    }

    auto reference_draw = [&reference, &reference_engine] { return reference(reference_engine); };
    auto selector_draw = [&selector, &selector_engine] { return selector(selector_engine); };
    const std::uint64_t reference_warmup = consume(warmup_iterations, reference_draw);
    const std::uint64_t selector_warmup = consume(warmup_iterations, selector_draw);
    const std::uint64_t reference_checksum =
        run_case("Fortuna 6.0.2 reference", "draw", iterations, reference_draw);
    const std::uint64_t selector_checksum =
        run_case("Storm::wide_index_selector", "draw", iterations, selector_draw);

    Storm::engine_type reference_construction_engine{seed};
    Storm::engine_type selector_construction_engine{seed};
    auto construct_reference = [&reference_construction_engine] {
        fortuna_6_0_2_compositional_reference constructed{
            reference_construction_engine, population_size};
        return constructed(reference_construction_engine);
    };
    auto construct_selector = [&selector_construction_engine] {
        Storm::wide_index_selector constructed{selector_construction_engine, population_size};
        return constructed(selector_construction_engine);
    };
    const std::uint64_t reference_construction_warmup =
        consume(construction_warmup, construct_reference);
    const std::uint64_t selector_construction_warmup =
        consume(construction_warmup, construct_selector);
    const std::uint64_t reference_construction_checksum = run_case(
        "Fortuna 6.0.2 reference construct + draw",
        "call",
        construction_iterations,
        construct_reference);
    const std::uint64_t selector_construction_checksum = run_case(
        "wide_index_selector construct + draw",
        "call",
        construction_iterations,
        construct_selector);

    const bool equivalent = reference_warmup == selector_warmup &&
                            reference_checksum == selector_checksum &&
                            reference_construction_warmup == selector_construction_warmup &&
                            reference_construction_checksum == selector_construction_checksum &&
                            reference_engine == selector_engine &&
                            reference_construction_engine == selector_construction_engine;
    const std::array checksums{
        reference_warmup,
        selector_warmup,
        reference_checksum,
        selector_checksum,
        reference_construction_warmup,
        selector_construction_warmup,
        reference_construction_checksum,
        selector_construction_checksum,
    };
    std::uint64_t combined_checksum = 0;
    for (const std::uint64_t checksum : checksums) {
        combined_checksum = mix(combined_checksum ^ checksum);
    }
    std::cout << "\ncombined checksum=" << combined_checksum
              << "\nreference equivalence=" << (equivalent ? "passed" : "FAILED") << '\n';
    return equivalent ? 0 : 1;
}
