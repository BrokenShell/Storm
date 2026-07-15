// SPDX-License-Identifier: MIT
#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <numbers>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Storm {

using Float = double;
using Integer = long long;
using Unsigned = unsigned long long;
using engine_type = std::mt19937_64;

inline constexpr char version[] = "5.0.0";

#define STORM_DEPRECATED(message) [[deprecated(message)]]

namespace detail {

inline constexpr Unsigned sign_bit = Unsigned{1} << 63U;
static_assert(sizeof(Integer) == sizeof(Unsigned));
static_assert(sizeof(Unsigned) == sizeof(engine_type::result_type));

inline constexpr auto signed_key(const Integer value) noexcept -> Unsigned {
    return std::bit_cast<Unsigned>(value) ^ sign_bit;
}

inline constexpr auto signed_from_key(const Unsigned key) noexcept -> Integer {
    return std::bit_cast<Integer>(key ^ sign_bit);
}

inline constexpr auto magnitude(const Integer value) noexcept -> Unsigned {
    const auto bits = static_cast<Unsigned>(value);
    return value < 0 ? Unsigned{0} - bits : bits;
}

inline auto bounded(engine_type& engine, const Unsigned bound) noexcept -> Unsigned {
    if (bound == 0) {
        return engine();
    }
    const Unsigned threshold = (Unsigned{0} - bound) % bound;
    for (;;) {
        const Unsigned value = engine();
        if (value >= threshold) {
            return value % bound;
        }
    }
}

inline void require_finite(const Float value, const char* const name) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument{name};
    }
}

inline void require_positive(const Float value, const char* const name) {
    require_finite(value, name);
    if (!(value > 0.0)) {
        throw std::invalid_argument{name};
    }
}

inline void require_probability(const Float value, const char* const name) {
    require_finite(value, name);
    if (value < 0.0 || value > 1.0) {
        throw std::invalid_argument{name};
    }
}

inline auto negative_value(const Unsigned positive_magnitude) noexcept -> Integer {
    if (positive_magnitude == sign_bit) {
        return std::numeric_limits<Integer>::lowest();
    }
    return -static_cast<Integer>(positive_magnitude);
}

inline auto legacy_index_value(const Integer size, const Unsigned offset) noexcept -> Integer {
    if (size > 0) {
        return static_cast<Integer>(offset);
    }
    return std::bit_cast<Integer>(~offset);
}

inline auto front_linear_offset(engine_type& engine, const Unsigned size) noexcept -> Unsigned {
    return std::min(bounded(engine, size), bounded(engine, size));
}

inline auto back_linear_offset(engine_type& engine, const Unsigned size) noexcept -> Unsigned {
    return std::max(bounded(engine, size), bounded(engine, size));
}

inline auto middle_linear_offset(engine_type& engine, const Unsigned size) noexcept -> Unsigned {
    const Unsigned first = bounded(engine, size);
    const Unsigned second = bounded(engine, size);
    return (first & second) + ((first ^ second) >> 1U);
}

inline auto checked_profile_offset(const Float value, const Unsigned size) noexcept -> Unsigned {
    if (!std::isfinite(value) || value < 0.0 || value >= static_cast<Float>(size)) {
        return size;
    }
    return static_cast<Unsigned>(value);
}

inline auto stable_ratio(const Float first, const Float second) noexcept -> Float {
    if (first >= second) return 1.0 / (1.0 + second / first);
    const Float ratio = first / second;
    return ratio / (1.0 + ratio);
}

inline auto symmetric_linear(engine_type& engine, const Unsigned bound) noexcept -> Integer {
    const Unsigned first = bounded(engine, bound + Unsigned{1});
    const Unsigned second = bounded(engine, bound + Unsigned{1});
    if (first >= second) return static_cast<Integer>(first - second);
    return -static_cast<Integer>(second - first);
}

}  // namespace detail

namespace detail {
inline void seed_from_entropy(engine_type& engine) {
    std::random_device source;
    std::array<std::uint32_t, 16> words{};
    for (auto& word : words) {
        word = static_cast<std::uint32_t>(source());
    }
    std::seed_seq sequence{words.begin(), words.end()};
    engine.seed(sequence);
}
}  // namespace detail

class Generator {
public:
    explicit Generator(const Unsigned seed_value = 0) : engine_{seed_value} {}

    auto engine() noexcept -> engine_type& { return engine_; }
    auto engine() const noexcept -> const engine_type& { return engine_; }
    void seed(const Unsigned seed_value) { engine_.seed(seed_value); }
    void reseed_from_entropy() { detail::seed_from_entropy(engine_); }

private:
    engine_type engine_;
};

inline auto thread_engine() -> engine_type& {
    thread_local engine_type engine{0};
    return engine;
}

inline void seed(const Unsigned seed_value) { thread_engine().seed(seed_value); }
inline void reseed_from_entropy() { detail::seed_from_entropy(thread_engine()); }

inline auto canonical(engine_type& engine) noexcept -> Float {
    constexpr Float scale = 0x1.0p-53;
    return static_cast<Float>(engine() >> 11U) * scale;
}

inline auto canonical() -> Float { return canonical(thread_engine()); }

inline auto uniform_unsigned(engine_type& engine, const Unsigned low, const Unsigned high)
    -> Unsigned {
    if (low > high) {
        throw std::invalid_argument{"uniform_unsigned requires low <= high"};
    }
    const Unsigned span = high - low;
    return low + detail::bounded(engine, span + Unsigned{1});
}

inline auto uniform_unsigned(const Unsigned low, const Unsigned high) -> Unsigned {
    return uniform_unsigned(thread_engine(), low, high);
}

inline auto uniform_integer(engine_type& engine, const Integer low, const Integer high)
    -> Integer {
    if (low > high) {
        throw std::invalid_argument{"uniform_integer requires low <= high"};
    }
    const Unsigned low_key = detail::signed_key(low);
    const Unsigned span = detail::signed_key(high) - low_key;
    return detail::signed_from_key(low_key + detail::bounded(engine, span + Unsigned{1}));
}

inline auto uniform_integer(const Integer low, const Integer high) -> Integer {
    return uniform_integer(thread_engine(), low, high);
}

inline auto uniform_index(engine_type& engine, const std::size_t size) -> std::size_t {
    static_assert(sizeof(std::size_t) <= sizeof(Unsigned));
    if (size == 0) {
        throw std::invalid_argument{"uniform_index requires a nonzero size"};
    }
    return static_cast<std::size_t>(detail::bounded(engine, static_cast<Unsigned>(size)));
}

inline auto uniform_index(const std::size_t size) -> std::size_t {
    return uniform_index(thread_engine(), size);
}

inline auto random_range(engine_type& engine,
                         const Integer start,
                         const Integer stop,
                         const Integer step) -> Integer {
    if (step == 0) {
        throw std::invalid_argument{"random_range step must not be zero"};
    }
    const Unsigned start_key = detail::signed_key(start);
    if (step > 0) {
        if (start >= stop) {
            throw std::invalid_argument{"random_range is empty for this positive step"};
        }
        const Unsigned stride = static_cast<Unsigned>(step);
        const Unsigned span = detail::signed_key(stop) - start_key;
        const Unsigned count = ((span - Unsigned{1}) / stride) + Unsigned{1};
        const Unsigned offset = detail::bounded(engine, count);
        return detail::signed_from_key(start_key + offset * stride);
    }
    if (start <= stop) {
        throw std::invalid_argument{"random_range is empty for this negative step"};
    }
    const Unsigned stride = Unsigned{0} - static_cast<Unsigned>(step);
    const Unsigned span = start_key - detail::signed_key(stop);
    const Unsigned count = ((span - Unsigned{1}) / stride) + Unsigned{1};
    const Unsigned offset = detail::bounded(engine, count);
    return detail::signed_from_key(start_key - offset * stride);
}

inline auto random_range(const Integer start, const Integer stop, const Integer step) -> Integer {
    return random_range(thread_engine(), start, stop, step);
}

inline auto roll_die(engine_type& engine, const std::size_t sides) -> std::size_t {
    if (sides == 0) {
        throw std::invalid_argument{"roll_die requires at least one side"};
    }
    return static_cast<std::size_t>(detail::bounded(engine, static_cast<Unsigned>(sides))) + 1U;
}

inline auto roll_die(const std::size_t sides) -> std::size_t {
    return roll_die(thread_engine(), sides);
}

inline auto roll_dice(engine_type& engine, const std::size_t rolls, const std::size_t sides)
    -> Unsigned {
    if (sides == 0) {
        throw std::invalid_argument{"roll_dice requires at least one side"};
    }
    const auto roll_count = static_cast<Unsigned>(rolls);
    const auto side_count = static_cast<Unsigned>(sides);
    if (roll_count != 0 && side_count > std::numeric_limits<Unsigned>::max() / roll_count) {
        throw std::overflow_error{"roll_dice result is not representable"};
    }
    Unsigned total = 0;
    for (std::size_t index = 0; index < rolls; ++index) {
        total += static_cast<Unsigned>(roll_die(engine, sides));
    }
    return total;
}

inline auto roll_dice(const std::size_t rolls, const std::size_t sides) -> Unsigned {
    return roll_dice(thread_engine(), rolls, sides);
}

inline auto ability_dice(engine_type& engine, const std::size_t dice_count) -> Unsigned {
    if (dice_count < 3) {
        throw std::invalid_argument{"ability_dice requires at least three dice"};
    }
    std::array<std::size_t, 3> best{};
    for (std::size_t index = 0; index < dice_count; ++index) {
        const std::size_t value = roll_die(engine, 6);
        if (value > best[0]) {
            best[0] = value;
            std::sort(best.begin(), best.end());
        }
        if (best[0] == 6) break;
    }
    return static_cast<Unsigned>(best[0] + best[1] + best[2]);
}

inline auto ability_dice(const std::size_t dice_count) -> Unsigned {
    return ability_dice(thread_engine(), dice_count);
}

namespace Engine {
using Twister STORM_DEPRECATED("use Storm::engine_type") = engine_type;
using Typhoon STORM_DEPRECATED("use Storm::engine_type") = engine_type;

STORM_DEPRECATED("use Storm::seed") inline void seed(const Unsigned seed_value) {
    Storm::seed(seed_value);
}
STORM_DEPRECATED("use Storm::reseed_from_entropy") inline void reseed_from_entropy() {
    Storm::reseed_from_entropy();
}
}  // namespace Engine

namespace GearBox {
template<typename Number>
STORM_DEPRECATED("compatibility helper; prefer std::clamp with ordered bounds")
inline auto clamp(const Number target, const Number left, const Number right) -> Number {
    return std::clamp(target, std::min(left, right), std::max(left, right));
}

template<typename Callable>
STORM_DEPRECATED("legacy compatibility helper")
inline auto approximation_clamp(Callable&& approximate,
                                const Integer target,
                                const Integer upper_bound) -> Integer {
    if (upper_bound <= 0) {
        throw std::invalid_argument{"upper_bound must be positive"};
    }
    return target >= 0 && target < upper_bound
               ? target
               : std::invoke(std::forward<Callable>(approximate), upper_bound);
}

template<typename Callable>
STORM_DEPRECATED("legacy compatibility helper")
inline auto analytic_continuation(Callable&& function,
                                  const Integer input,
                                  const Integer offset) -> Integer {
    if (input == std::numeric_limits<Integer>::lowest()) {
        throw std::overflow_error{"analytic continuation cannot negate LLONG_MIN"};
    }
    if (input > 0) {
        return std::invoke(std::forward<Callable>(function), input);
    }
    if (input < 0) {
        const Integer value = std::invoke(std::forward<Callable>(function), -input);
        if (value == std::numeric_limits<Integer>::lowest()) {
            throw std::overflow_error{"analytic continuation result cannot be negated"};
        }
        const Integer negated = -value;
        if ((offset > 0 && negated > std::numeric_limits<Integer>::max() - offset) ||
            (offset < 0 && negated < std::numeric_limits<Integer>::lowest() - offset)) {
            throw std::overflow_error{"analytic continuation offset is not representable"};
        }
        return negated + offset;
    }
    return offset;
}
}  // namespace GearBox

namespace Meters {
STORM_DEPRECATED("use std::numeric_limits") inline constexpr auto max_uint() noexcept -> Unsigned {
    return std::numeric_limits<Unsigned>::max();
}
STORM_DEPRECATED("use std::numeric_limits") inline constexpr auto min_int() noexcept -> Integer {
    return std::numeric_limits<Integer>::lowest();
}
STORM_DEPRECATED("use std::numeric_limits") inline constexpr auto max_int() noexcept -> Integer {
    return std::numeric_limits<Integer>::max();
}
STORM_DEPRECATED("use std::numeric_limits") inline constexpr auto min_float() noexcept -> Float {
    return std::numeric_limits<Float>::lowest();
}
STORM_DEPRECATED("use std::numeric_limits") inline constexpr auto max_float() noexcept -> Float {
    return std::numeric_limits<Float>::max();
}
STORM_DEPRECATED("use std::nextafter") inline auto min_below() noexcept -> Float {
    return std::nextafter(0.0, std::numeric_limits<Float>::lowest());
}
STORM_DEPRECATED("use std::nextafter") inline auto min_above() noexcept -> Float {
    return std::nextafter(0.0, std::numeric_limits<Float>::max());
}
}  // namespace Meters

namespace GetBool {
STORM_DEPRECATED("use an injected standard distribution")
inline auto bernoulli_variate(const Float probability) -> bool {
    detail::require_probability(probability, "bernoulli probability must be finite and in [0,1]");
    if (probability == 0.0) return false;
    if (probability == 1.0) return true;
    std::bernoulli_distribution distribution{probability};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use bernoulli probability in [0,1]")
inline auto percent_true(const Float percentage) -> bool {
    detail::require_finite(percentage, "percentage must be finite");
    if (percentage < 0.0 || percentage > 100.0) {
        throw std::invalid_argument{"percentage must be in [0,100]"};
    }
    if (percentage == 0.0) return false;
    if (percentage == 100.0) return true;
    std::bernoulli_distribution distribution{percentage / 100.0};
    return distribution(thread_engine());
}
}  // namespace GetBool

namespace GetFloat {
STORM_DEPRECATED("use Storm::canonical") inline auto canonical_variate() -> Float {
    return canonical();
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto uniform_real_variate(const Float low, const Float high) -> Float {
    detail::require_finite(low, "uniform low must be finite");
    detail::require_finite(high, "uniform high must be finite");
    if (!(low < high)) throw std::invalid_argument{"uniform real interval must be nonempty"};
    if (!std::isfinite(high - low)) {
        throw std::overflow_error{"uniform real interval width is not representable"};
    }
    std::uniform_real_distribution<Float> distribution{low, high};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto exponential_variate(const Float rate) -> Float {
    detail::require_positive(rate, "exponential rate must be positive and finite");
    std::exponential_distribution<Float> distribution{rate};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto gamma_variate(const Float shape, const Float scale) -> Float {
    detail::require_positive(shape, "gamma shape must be positive and finite");
    detail::require_positive(scale, "gamma scale must be positive and finite");
    std::gamma_distribution<Float> distribution{shape, scale};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto weibull_variate(const Float shape, const Float scale) -> Float {
    detail::require_positive(shape, "weibull shape must be positive and finite");
    detail::require_positive(scale, "weibull scale must be positive and finite");
    std::weibull_distribution<Float> distribution{shape, scale};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto normal_variate(const Float mean, const Float deviation) -> Float {
    detail::require_finite(mean, "normal mean must be finite");
    detail::require_positive(deviation, "normal deviation must be positive and finite");
    std::normal_distribution<Float> distribution{mean, deviation};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto log_normal_variate(const Float mean, const Float deviation) -> Float {
    detail::require_finite(mean, "lognormal mean must be finite");
    detail::require_positive(deviation, "lognormal deviation must be positive and finite");
    std::lognormal_distribution<Float> distribution{mean, deviation};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto extreme_value_variate(const Float location, const Float scale) -> Float {
    detail::require_finite(location, "extreme-value location must be finite");
    detail::require_positive(scale, "extreme-value scale must be positive and finite");
    std::extreme_value_distribution<Float> distribution{location, scale};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto chi_squared_variate(const Float degrees) -> Float {
    detail::require_positive(degrees, "chi-squared degrees must be positive and finite");
    std::chi_squared_distribution<Float> distribution{degrees};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto cauchy_variate(const Float location, const Float scale) -> Float {
    detail::require_finite(location, "cauchy location must be finite");
    detail::require_positive(scale, "cauchy scale must be positive and finite");
    std::cauchy_distribution<Float> distribution{location, scale};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto fisher_f_variate(const Float first_degrees, const Float second_degrees) -> Float {
    detail::require_positive(first_degrees, "F degrees must be positive and finite");
    detail::require_positive(second_degrees, "F degrees must be positive and finite");
    std::fisher_f_distribution<Float> distribution{first_degrees, second_degrees};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use an injected standard distribution")
inline auto student_t_variate(const Float degrees) -> Float {
    detail::require_positive(degrees, "Student t degrees must be positive and finite");
    std::student_t_distribution<Float> distribution{degrees};
    return distribution(thread_engine());
}

STORM_DEPRECATED("legacy beta recipe")
inline auto beta_variate(const Float alpha, const Float beta) -> Float {
    detail::require_positive(alpha, "beta alpha must be positive and finite");
    detail::require_positive(beta, "beta beta must be positive and finite");
    constexpr Float direct_sampling_limit = 1.0e6;
    if (alpha > direct_sampling_limit || beta > direct_sampling_limit) {
        return detail::stable_ratio(alpha, beta);
    }
    std::gamma_distribution<Float> first_distribution{alpha, 1.0};
    std::gamma_distribution<Float> second_distribution{beta, 1.0};
    for (int attempt = 0; attempt < 64; ++attempt) {
        const Float first = first_distribution(thread_engine());
        const Float second = second_distribution(thread_engine());
        if (first == 0.0 && second == 0.0) continue;
        if (std::isinf(first)) return std::isinf(second) ? detail::stable_ratio(alpha, beta) : 1.0;
        if (std::isinf(second)) return 0.0;
        return detail::stable_ratio(first, second);
    }
    return detail::stable_ratio(alpha, beta);
}

STORM_DEPRECATED("legacy Pareto recipe")
inline auto pareto_variate(const Float alpha) -> Float {
    detail::require_positive(alpha, "Pareto alpha must be positive and finite");
    const Float value = 1.0 / std::pow(1.0 - canonical(), 1.0 / alpha);
    if (!std::isfinite(value)) throw std::overflow_error{"Pareto result is not finite"};
    return value;
}

STORM_DEPRECATED("legacy von Mises recipe")
inline auto vonmises_variate(const Float mean, const Float kappa) -> Float {
    detail::require_finite(mean, "von Mises mean must be finite");
    detail::require_finite(kappa, "von Mises kappa must be finite");
    if (kappa < 0.0) throw std::invalid_argument{"von Mises kappa must be nonnegative"};
    constexpr Float tau = 2.0 * std::numbers::pi_v<Float>;
    if (kappa < 1.0e-6) return tau * canonical();
    if (kappa > std::sqrt(std::numeric_limits<Float>::max()) / 2.0) {
        throw std::overflow_error{"von Mises intermediate arithmetic is not representable"};
    }
    const Float reduced_mean = std::fmod(mean, tau);
    const Float a = 1.0 + std::sqrt(1.0 + 4.0 * kappa * kappa);
    const Float b = (a - std::sqrt(2.0 * a)) / (2.0 * kappa);
    const Float r = (1.0 + b * b) / (2.0 * b);
    for (;;) {
        const Float z = std::cos(std::numbers::pi_v<Float> * canonical());
        const Float f = (1.0 + r * z) / (r + z);
        const Float c = kappa * (r - f);
        const Float second = canonical();
        if (second < c * (2.0 - c) || second <= c * std::exp(1.0 - c)) {
            Float theta = canonical() < 0.5 ? std::acos(f) : -std::acos(f);
            theta = std::fmod(theta + reduced_mean, tau);
            return theta < 0.0 ? theta + tau : theta;
        }
    }
}

STORM_DEPRECATED("legacy triangular recipe")
inline auto triangular_variate(const Float low, const Float high, const Float mode) -> Float {
    detail::require_finite(low, "triangular low must be finite");
    detail::require_finite(high, "triangular high must be finite");
    detail::require_finite(mode, "triangular mode must be finite");
    if (!(low < high) || mode < low || mode > high) {
        throw std::invalid_argument{"triangular requires low < high and low <= mode <= high"};
    }
    const Float width = high - low;
    if (!std::isfinite(width)) {
        throw std::overflow_error{"triangular interval width is not representable"};
    }
    const Float value = canonical();
    const Float split = (mode - low) / width;
    if (value > split) {
        return high - std::sqrt(1.0 - value) * std::sqrt(width) * std::sqrt(high - mode);
    }
    return low + std::sqrt(value) * std::sqrt(width) * std::sqrt(mode - low);
}
}  // namespace GetFloat

namespace GetInt {
STORM_DEPRECATED("use Storm::uniform_unsigned")
inline auto uniform_uint_variate(const Unsigned first, const Unsigned second) -> Unsigned {
    return uniform_unsigned(std::min(first, second), std::max(first, second));
}

STORM_DEPRECATED("use Storm::uniform_integer")
inline auto uniform_int_variate(const Integer first, const Integer second) -> Integer {
    return uniform_integer(std::min(first, second), std::max(first, second));
}

STORM_DEPRECATED("use Storm::uniform_integer or uniform_index")
inline auto random_below(const Integer number) -> Integer {
    if (number > 0) return uniform_integer(0, number - 1);
    if (number < 0) return uniform_integer(number + 1, 0);
    throw std::invalid_argument{"random_below requires a nonzero bound"};
}

STORM_DEPRECATED("use Storm::random_range for directed semantics")
inline auto random_range(const Integer start, const Integer stop, const Integer step) -> Integer {
    if (step == 0) throw std::invalid_argument{"random_range step must not be zero"};
    if (start == stop) throw std::invalid_argument{"random_range interval must be nonempty"};
    const Integer low = std::min(start, stop);
    const Integer high = std::max(start, stop);
    const Unsigned low_key = detail::signed_key(low);
    const Unsigned high_key = detail::signed_key(high);
    const Unsigned span = high_key - low_key;
    const Unsigned stride = detail::magnitude(step);
    const Unsigned count = ((span - Unsigned{1}) / stride) + Unsigned{1};
    const Unsigned offset = detail::bounded(thread_engine(), count);
    return step > 0 ? detail::signed_from_key(low_key + offset * stride)
                    : detail::signed_from_key(high_key - offset * stride);
}

STORM_DEPRECATED("use a validated standard distribution")
inline auto binomial_variate(const Integer trials, const Float probability) -> Integer {
    if (trials < 0) throw std::invalid_argument{"binomial trials must be nonnegative"};
    detail::require_probability(probability, "binomial probability must be in [0,1]");
    if (trials == 0 || probability == 0.0) return 0;
    if (probability == 1.0) return trials;
    std::binomial_distribution<Integer> distribution{trials, probability};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use a validated standard distribution")
inline auto negative_binomial_variate(const Integer successes, const Float probability) -> Integer {
    if (successes <= 0) throw std::invalid_argument{"negative-binomial successes must be positive"};
    detail::require_probability(probability, "negative-binomial probability must be in (0,1]");
    if (probability == 0.0) throw std::invalid_argument{"negative-binomial probability must be positive"};
    if (probability == 1.0) return 0;
    std::negative_binomial_distribution<Integer> distribution{successes, probability};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use a validated standard distribution")
inline auto geometric_variate(const Float probability) -> Integer {
    detail::require_probability(probability, "geometric probability must be in (0,1)");
    if (!(probability > 0.0 && probability < 1.0)) {
        throw std::invalid_argument{"geometric probability must be in (0,1)"};
    }
    std::geometric_distribution<Integer> distribution{probability};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use a validated standard distribution")
inline auto poisson_variate(const Float mean) -> Integer {
    detail::require_positive(mean, "Poisson mean must be positive and finite");
    const Float largest_representable_mean = std::nextafter(
        static_cast<Float>(std::numeric_limits<Integer>::max()), 0.0);
    if (mean > largest_representable_mean) {
        throw std::overflow_error{"Poisson mean exceeds the integer result domain"};
    }
    std::poisson_distribution<Integer> distribution{mean};
    return distribution(thread_engine());
}

STORM_DEPRECATED("use Storm::roll_die")
inline auto d(const Integer sides) -> Integer {
    if (sides == 0) return 0;
    const Unsigned size = detail::magnitude(sides);
    const Unsigned value = detail::bounded(thread_engine(), size) + Unsigned{1};
    return sides > 0 ? static_cast<Integer>(value) : detail::negative_value(value);
}

STORM_DEPRECATED("use Storm::roll_dice")
inline auto dice(const Integer rolls, const Integer sides) -> Integer {
    if (rolls == 0 || sides == 0) return 0;
    const Unsigned roll_count = detail::magnitude(rolls);
    const Unsigned side_count = detail::magnitude(sides);
    const bool negative = (rolls < 0) != (sides < 0);
    const Unsigned magnitude_limit = negative
                                         ? detail::sign_bit
                                         : static_cast<Unsigned>(std::numeric_limits<Integer>::max());
    if (side_count > magnitude_limit / roll_count) {
        throw std::overflow_error{"legacy dice result is not representable"};
    }
    if (side_count == 1) {
        return negative ? detail::negative_value(roll_count)
                        : static_cast<Integer>(roll_count);
    }
    Unsigned total = 0;
    for (Unsigned index = 0; index < roll_count; ++index) {
        total += detail::bounded(thread_engine(), side_count) + Unsigned{1};
    }
    return negative ? detail::negative_value(total) : static_cast<Integer>(total);
}

STORM_DEPRECATED("use Storm::ability_dice")
inline auto ability_dice(const Integer number) -> Integer {
    const Integer clamped = std::clamp<Integer>(number, 3, 9);
    return static_cast<Integer>(Storm::ability_dice(static_cast<std::size_t>(clamped)));
}

STORM_DEPRECATED("legacy compatibility recipe")
inline auto plus_or_minus(const Integer number) -> Integer {
    if (number == std::numeric_limits<Integer>::lowest()) {
        throw std::overflow_error{"a symmetric LLONG_MIN interval is not representable"};
    }
    const Integer bound = number < 0 ? -number : number;
    return uniform_integer(-bound, bound);
}

STORM_DEPRECATED("legacy compatibility recipe")
inline auto plus_or_minus_linear(const Integer number) -> Integer {
    if (number == std::numeric_limits<Integer>::lowest()) {
        throw std::overflow_error{"a symmetric LLONG_MIN interval is not representable"};
    }
    const Unsigned bound = static_cast<Unsigned>(number < 0 ? -number : number);
    return detail::symmetric_linear(thread_engine(), bound);
}

STORM_DEPRECATED("legacy compatibility recipe")
inline auto plus_or_minus_gauss(const Integer number) -> Integer {
    if (number == std::numeric_limits<Integer>::lowest()) {
        throw std::overflow_error{"a symmetric LLONG_MIN interval is not representable"};
    }
    const Integer bound = number < 0 ? -number : number;
    if (bound == 0) return 0;
    std::normal_distribution<Float> distribution{0.0, static_cast<Float>(bound) / std::numbers::pi_v<Float>};
    const Float sample = std::round(distribution(thread_engine()));
    if (std::isfinite(sample) && sample >= -static_cast<Float>(bound) &&
        sample <= static_cast<Float>(bound) &&
        sample > static_cast<Float>(std::numeric_limits<Integer>::lowest()) &&
        sample < static_cast<Float>(std::numeric_limits<Integer>::max())) {
        return static_cast<Integer>(sample);
    }
    return detail::symmetric_linear(thread_engine(), static_cast<Unsigned>(bound));
}
}  // namespace GetInt

namespace GetIndex {
namespace index_detail {
inline auto require_size(const Integer number) -> Unsigned {
    if (number == 0) throw std::invalid_argument{"legacy index size must be nonzero"};
    return detail::magnitude(number);
}

inline auto linear_offset(engine_type& engine, const Unsigned size, const int profile) -> Unsigned {
    if (profile < 0) return detail::front_linear_offset(engine, size);
    if (profile > 0) return detail::back_linear_offset(engine, size);
    return detail::middle_linear_offset(engine, size);
}

inline auto profile_offset(engine_type& engine,
                           const Unsigned size,
                           const int profile,
                           const int family) -> Unsigned {
    constexpr Unsigned exact_double_limit = Unsigned{1} << 53U;
    if (size >= exact_double_limit) return linear_offset(engine, size, profile);
    const Float scale = static_cast<Float>(size);
    Float sample = 0.0;
    if (family == 1) {
        if (profile < 0) {
            std::gamma_distribution<Float> distribution{1.0, scale / 10.0};
            sample = std::floor(distribution(engine));
        } else if (profile > 0) {
            std::gamma_distribution<Float> distribution{1.0, scale / 10.0};
            const Unsigned front = detail::checked_profile_offset(std::floor(distribution(engine)), size);
            return front < size ? size - front - Unsigned{1} : linear_offset(engine, size, profile);
        } else {
            std::normal_distribution<Float> distribution{scale / 2.0, scale / 10.0};
            sample = std::floor(distribution(engine));
        }
    } else {
        std::poisson_distribution<Unsigned> distribution{scale / 4.0};
        const Unsigned front = distribution(engine);
        if (profile < 0) return front < size ? front : linear_offset(engine, size, profile);
        if (profile > 0) return front < size ? size - front - Unsigned{1} : linear_offset(engine, size, profile);
        if (detail::bounded(engine, 2) == 0) return front < size ? front : linear_offset(engine, size, -1);
        return front < size ? size - front - Unsigned{1} : linear_offset(engine, size, 1);
    }
    const Unsigned result = detail::checked_profile_offset(sample, size);
    return result < size ? result : linear_offset(engine, size, profile);
}

inline auto apply(const Integer number, const int profile, const int family) -> Integer {
    const Unsigned size = require_size(number);
    const int oriented_profile = number < 0 ? -profile : profile;
    Unsigned offset = 0;
    if (family == 0) offset = linear_offset(thread_engine(), size, oriented_profile);
    else offset = profile_offset(thread_engine(), size, oriented_profile, family);
    return detail::legacy_index_value(number, offset);
}
}  // namespace index_detail

STORM_DEPRECATED("use Storm::uniform_index") inline auto random_index(const Integer number) -> Integer {
    const Unsigned size = index_detail::require_size(number);
    return detail::legacy_index_value(number, detail::bounded(thread_engine(), size));
}
STORM_DEPRECATED("legacy custom profile") inline auto front_linear(const Integer number) -> Integer {
    return index_detail::apply(number, -1, 0);
}
STORM_DEPRECATED("legacy custom profile") inline auto middle_linear(const Integer number) -> Integer {
    return index_detail::apply(number, 0, 0);
}
STORM_DEPRECATED("legacy custom profile") inline auto back_linear(const Integer number) -> Integer {
    return index_detail::apply(number, 1, 0);
}
STORM_DEPRECATED("legacy custom profile") inline auto quantum_linear(const Integer number) -> Integer {
    const int profile = static_cast<int>(detail::bounded(thread_engine(), 3)) - 1;
    return index_detail::apply(number, profile, 0);
}
STORM_DEPRECATED("legacy custom profile") inline auto front_gauss(const Integer number) -> Integer {
    return index_detail::apply(number, -1, 1);
}
STORM_DEPRECATED("legacy custom profile") inline auto middle_gauss(const Integer number) -> Integer {
    return index_detail::apply(number, 0, 1);
}
STORM_DEPRECATED("legacy custom profile") inline auto back_gauss(const Integer number) -> Integer {
    return index_detail::apply(number, 1, 1);
}
STORM_DEPRECATED("legacy custom profile") inline auto quantum_gauss(const Integer number) -> Integer {
    const int profile = static_cast<int>(detail::bounded(thread_engine(), 3)) - 1;
    return index_detail::apply(number, profile, 1);
}
STORM_DEPRECATED("legacy custom profile") inline auto front_poisson(const Integer number) -> Integer {
    return index_detail::apply(number, -1, 2);
}
STORM_DEPRECATED("legacy custom profile") inline auto middle_poisson(const Integer number) -> Integer {
    return index_detail::apply(number, 0, 2);
}
STORM_DEPRECATED("legacy custom profile") inline auto back_poisson(const Integer number) -> Integer {
    return index_detail::apply(number, 1, 2);
}
STORM_DEPRECATED("legacy custom profile") inline auto quantum_poisson(const Integer number) -> Integer {
    const int profile = static_cast<int>(detail::bounded(thread_engine(), 3)) - 1;
    return index_detail::apply(number, profile, 2);
}
STORM_DEPRECATED("legacy custom profile") inline auto quantum_monty(const Integer number) -> Integer {
    const int family = static_cast<int>(detail::bounded(thread_engine(), 3));
    const int profile = static_cast<int>(detail::bounded(thread_engine(), 3)) - 1;
    return index_detail::apply(number, profile, family);
}
}  // namespace GetIndex

STORM_DEPRECATED("use Storm::version") inline constexpr auto get_version() noexcept -> const char* {
    return version;
}

#undef STORM_DEPRECATED

}  // namespace Storm
