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
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

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
              const std::string_view unit,
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
    const auto nanoseconds_per_unit = elapsed / static_cast<double>(iterations);

    std::cout << std::left << std::setw(38) << label << std::right << std::fixed
              << std::setprecision(2) << std::setw(12) << nanoseconds_per_unit
              << " ns/" << unit << "  checksum=" << checksum << '\n';
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

class LinearPreparedWeightedIndex {
public:
    explicit LinearPreparedWeightedIndex(const std::vector<double>& weights) {
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

auto weighted_benchmark(const std::size_t size,
                        const std::size_t iterations,
                        const std::size_t warmup_iterations,
                        std::uint64_t& warmup_checksum) -> std::uint64_t {
    std::vector<double> weights;
    weights.reserve(size);
    for (std::size_t index = 0; index < size; ++index) {
        const double weight =
            index % 11U == 0U ? 0.0 : static_cast<double>((index % 7U) + 1U);
        weights.push_back(weight);
    }

    const Storm::PreparedWeightedIndex prepared{weights};
    Storm::Generator prepared_generator{seed};
    auto prepared_draw = [&prepared, &prepared_generator] {
        return prepared(prepared_generator.engine());
    };
    warmup_checksum ^= mix(warm_up(warmup_iterations, prepared_draw) +
                           static_cast<std::uint64_t>(size));
    const std::string prepared_label =
        "PreparedWeightedIndex (" + std::to_string(size) + " entries)";
    const auto prepared_checksum =
        run_case(prepared_label, "draw", iterations, prepared_draw);

    const LinearPreparedWeightedIndex linear{weights};
    Storm::Generator linear_generator{seed};
    auto linear_draw = [&linear, &linear_generator] {
        return linear(linear_generator.engine());
    };
    warmup_checksum ^= mix(warm_up(warmup_iterations, linear_draw) +
                           static_cast<std::uint64_t>(size * 2U));
    const std::string linear_label =
        "linear prepared reference (" + std::to_string(size) + " entries)";
    const auto linear_checksum = run_case(linear_label, "draw", iterations, linear_draw);

    return prepared_checksum ^ mix(linear_checksum + static_cast<std::uint64_t>(size));
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
        "draw",
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
        "draw",
        iterations,
        standard_index);

    Storm::Generator storm_canonical_generator{seed};
    auto storm_canonical = [&storm_canonical_generator] {
        return Storm::canonical(storm_canonical_generator.engine());
    };
    warmup_checksum ^= warm_up(warmup_iterations, storm_canonical);
    const auto storm_canonical_checksum = run_case(
        "Storm::canonical",
        "draw",
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
        "draw",
        iterations,
        standard_canonical);

    Storm::Generator storm_ability_generator{seed};
    auto storm_ability = [&storm_ability_generator] {
        return Storm::ability_dice(storm_ability_generator.engine(), std::size_t{4});
    };
    warmup_checksum ^= warm_up(warmup_iterations, storm_ability);
    const auto storm_ability_checksum = run_case(
        "Storm::ability_dice (4d6 keep 3)",
        "call",
        iterations,
        storm_ability);

    std::uint64_t weighted_checksum = 0;
    for (const std::size_t size : std::array<std::size_t, 3>{4U, 100U, 1'000U}) {
        weighted_checksum ^=
            weighted_benchmark(size, iterations, warmup_iterations, warmup_checksum);
    }

    const auto combined_checksum = storm_index_checksum ^ standard_index_checksum ^
                                   storm_canonical_checksum ^ standard_canonical_checksum ^
                                   storm_ability_checksum ^ weighted_checksum;
    std::cout << "\nwarmup checksum=" << warmup_checksum
              << "\ncombined checksum=" << combined_checksum << '\n';
    return 0;
}
