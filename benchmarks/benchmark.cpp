// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include <algorithm>
#include <bit>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace {

constexpr std::uint64_t seed = 0x5A17'2026'0715'0001ULL;
constexpr std::size_t bound = 1'000;
constexpr std::size_t default_iterations = 2'000'000;

template<typename Value>
auto checksum_value(const Value value) -> std::uint64_t {
    if constexpr (std::is_floating_point_v<Value>) {
        static_assert(sizeof(Value) == sizeof(std::uint64_t));
        return std::bit_cast<std::uint64_t>(value);
    } else {
        return static_cast<std::uint64_t>(value);
    }
}

auto mix(const std::uint64_t value) -> std::uint64_t {
    auto mixed = value + 0x9E37'79B9'7F4A'7C15ULL;
    mixed = (mixed ^ (mixed >> 30U)) * 0xBF58'476D'1CE4'E5B9ULL;
    mixed = (mixed ^ (mixed >> 27U)) * 0x94D0'49BB'1331'11EBULL;
    return mixed ^ (mixed >> 31U);
}

template<typename Callable>
auto warm_up(const std::size_t iterations, Callable& callable) -> std::uint64_t {
    std::uint64_t checksum = 0;
    for (std::size_t index = 0; index < iterations; ++index) {
        checksum ^= mix(checksum_value(callable()) + static_cast<std::uint64_t>(index));
    }
    return checksum;
}

template<typename Callable>
auto run_case(const std::string_view label,
              const std::size_t iterations,
              Callable&& callable) -> std::uint64_t {
    std::uint64_t checksum = 0;
    const auto start = std::chrono::steady_clock::now();
    for (std::size_t index = 0; index < iterations; ++index) {
        checksum ^= mix(checksum_value(callable()) +
                        static_cast<std::uint64_t>(index));
    }
    const auto stop = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration<double, std::nano>{stop - start}.count();
    const auto nanoseconds_per_draw = elapsed / static_cast<double>(iterations);

    std::cout << std::left << std::setw(38) << label << std::right << std::fixed
              << std::setprecision(2) << std::setw(12) << nanoseconds_per_draw
              << " ns/draw  checksum=" << checksum << '\n';
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
        std::cerr << "usage: storm_benchmark [positive-iteration-count]\n";
        return 2;
    }

    const std::size_t warmup_iterations = std::max<std::size_t>(
        1,
        std::min<std::size_t>(iterations / 10, 50'000));

    std::cout << "Storm " << Storm::version << " benchmark\n"
              << "iterations=" << iterations << ", seed=" << seed << ", bound=" << bound
              << ", warmup=" << warmup_iterations
              << "\nResults are descriptive; no timing threshold is applied.\n\n";

    std::uint64_t warmup_checksum = 0;

    Storm::Generator storm_index_generator{seed};
    auto storm_index = [&storm_index_generator] {
        return Storm::uniform_index(storm_index_generator.engine(), bound);
    };
    warmup_checksum ^= warm_up(warmup_iterations, storm_index);
    const auto storm_index_checksum = run_case(
        "Storm::uniform_index",
        iterations,
        storm_index);

    std::mt19937_64 standard_index_generator{seed};
    std::uniform_int_distribution<std::size_t> standard_index_distribution{0, bound - 1};
    auto standard_index = [&standard_index_generator, &standard_index_distribution] {
        return standard_index_distribution(standard_index_generator);
    };
    warmup_checksum ^= warm_up(warmup_iterations, standard_index);
    const auto standard_index_checksum = run_case(
        "std::uniform_int_distribution",
        iterations,
        standard_index);

    Storm::Generator storm_canonical_generator{seed};
    auto storm_canonical = [&storm_canonical_generator] {
        return Storm::canonical(storm_canonical_generator.engine());
    };
    warmup_checksum ^= warm_up(warmup_iterations, storm_canonical);
    const auto storm_canonical_checksum = run_case(
        "Storm::canonical",
        iterations,
        storm_canonical);

    std::mt19937_64 standard_canonical_generator{seed};
    auto standard_canonical = [&standard_canonical_generator] {
        return std::generate_canonical<double, std::numeric_limits<double>::digits>(
            standard_canonical_generator);
    };
    warmup_checksum ^= warm_up(warmup_iterations, standard_canonical);
    const auto standard_canonical_checksum = run_case(
        "std::generate_canonical<double>",
        iterations,
        standard_canonical);

    const auto combined_checksum = storm_index_checksum ^ standard_index_checksum ^
                                   storm_canonical_checksum ^ standard_canonical_checksum;
    std::cout << "\nwarmup checksum=" << warmup_checksum
              << "\ncombined checksum=" << combined_checksum << '\n';
    return 0;
}
