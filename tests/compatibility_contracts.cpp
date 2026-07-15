// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace {

using Storm::Integer;
using IndexFunction = Integer (*)(Integer);

constexpr auto integer_min = std::numeric_limits<Integer>::lowest();
constexpr auto integer_max = std::numeric_limits<Integer>::max();

constexpr std::array<IndexFunction, 14> index_functions{
    Storm::GetIndex::random_index,
    Storm::GetIndex::front_linear,
    Storm::GetIndex::middle_linear,
    Storm::GetIndex::back_linear,
    Storm::GetIndex::quantum_linear,
    Storm::GetIndex::front_gauss,
    Storm::GetIndex::middle_gauss,
    Storm::GetIndex::back_gauss,
    Storm::GetIndex::quantum_gauss,
    Storm::GetIndex::front_poisson,
    Storm::GetIndex::middle_poisson,
    Storm::GetIndex::back_poisson,
    Storm::GetIndex::quantum_poisson,
    Storm::GetIndex::quantum_monty,
};

void test_meters_and_gearbox() {
    STORM_CHECK(std::string_view{Storm::get_version()} == "5.0.0");
    STORM_CHECK(Storm::Meters::min_int() == integer_min);
    STORM_CHECK(Storm::Meters::max_int() == integer_max);
    STORM_CHECK(Storm::Meters::max_uint() == std::numeric_limits<Storm::Unsigned>::max());
    STORM_CHECK(Storm::Meters::min_float() == std::numeric_limits<Storm::Float>::lowest());
    STORM_CHECK(Storm::Meters::max_float() == std::numeric_limits<Storm::Float>::max());
    STORM_CHECK(Storm::Meters::min_below() < 0.0);
    STORM_CHECK(Storm::Meters::min_above() > 0.0);
    STORM_CHECK(Storm::GearBox::clamp(3, 2, 1) == 2);
    STORM_CHECK(Storm::GearBox::clamp(0, 2, 1) == 1);
    const auto identity = [](const Integer value) { return value; };
    STORM_CHECK(Storm::GearBox::approximation_clamp(identity, 3, 10) == 3);
    STORM_CHECK(Storm::GearBox::approximation_clamp(identity, 12, 10) == 10);
    STORM_CHECK(Storm::GearBox::analytic_continuation(identity, 3, -1) == 3);
    STORM_CHECK(Storm::GearBox::analytic_continuation(identity, -3, -1) == -4);
    const auto throwing = [](Integer) -> Integer { throw std::runtime_error{"expected"}; };
    const auto minimum_result = [](Integer) { return integer_min; };
    STORM_EXPECT_THROWS(std::runtime_error,
                        Storm::GearBox::approximation_clamp(throwing, 12, 10));
    STORM_EXPECT_THROWS(std::overflow_error,
                        Storm::GearBox::analytic_continuation(identity, integer_min, -1));
    STORM_EXPECT_THROWS(std::overflow_error,
                        Storm::GearBox::analytic_continuation(minimum_result, -1, 0));
}

void test_legacy_uniforms_and_range_phase() {
    Storm::Engine::seed(Storm::Unsigned{21});
    for (int iteration = 0; iteration < 128; ++iteration) {
        const double value = Storm::GetFloat::canonical_variate();
        STORM_CHECK(value >= 0.0 && value < 1.0);
    }
    Storm::Engine::seed(Storm::Unsigned{0});
    const Integer zero_seed_first = Storm::GetInt::uniform_int_variate(integer_min, integer_max);
    Storm::Engine::seed(Storm::Unsigned{0});
    STORM_CHECK(Storm::GetInt::uniform_int_variate(integer_min, integer_max) == zero_seed_first);
    Storm::Engine::seed(Storm::Unsigned{21});
    for (int iteration = 0; iteration < 2048; ++iteration) {
        const auto ordered = Storm::GetInt::uniform_int_variate(-8, 5);
        const auto reversed = Storm::GetInt::uniform_int_variate(5, -8);
        STORM_CHECK(ordered >= -8 && ordered <= 5);
        STORM_CHECK(reversed >= -8 && reversed <= 5);
        STORM_CHECK(Storm::GetInt::uniform_uint_variate(Storm::Unsigned{9}, Storm::Unsigned{2}) >=
                    Storm::Unsigned{2});
        STORM_CHECK(Storm::GetInt::uniform_uint_variate(Storm::Unsigned{9}, Storm::Unsigned{2}) <=
                    Storm::Unsigned{9});
        STORM_CHECK(Storm::GetInt::random_below(1) == 0);
        const Integer negative_below = Storm::GetInt::random_below(-17);
        STORM_CHECK(negative_below > -17 && negative_below <= 0);

        const Integer positive_forward = Storm::GetInt::random_range(1, 10, 2);
        const Integer positive_reverse = Storm::GetInt::random_range(10, 1, 2);
        const Integer negative_forward = Storm::GetInt::random_range(1, 10, -2);
        const Integer negative_reverse = Storm::GetInt::random_range(10, 1, -2);
        STORM_CHECK(positive_forward >= 1 && positive_forward < 10 && positive_forward % 2 == 1);
        STORM_CHECK(positive_reverse >= 1 && positive_reverse < 10 && positive_reverse % 2 == 1);
        STORM_CHECK(negative_forward > 1 && negative_forward <= 10 && negative_forward % 2 == 0);
        STORM_CHECK(negative_reverse > 1 && negative_reverse <= 10 && negative_reverse % 2 == 0);
    }

    for (int iteration = 0; iteration < 128; ++iteration) {
        const Integer positive = Storm::GetInt::random_range(integer_min, integer_max, integer_max);
        const Integer negative = Storm::GetInt::random_range(integer_max, integer_min, -integer_max);
        const Integer minimum_step = Storm::GetInt::random_range(integer_min, integer_max, integer_min);
        STORM_CHECK(positive == integer_min || positive == -1 || positive == integer_max - 1);
        STORM_CHECK(negative == integer_max || negative == 0 || negative == -integer_max);
        STORM_CHECK(minimum_step == integer_max || minimum_step == -1);
    }

    STORM_EXPECT_THROWS(std::invalid_argument, Storm::GetInt::random_below(0));
    const Integer minimum_below = Storm::GetInt::random_below(integer_min);
    STORM_CHECK(minimum_below > integer_min && minimum_below <= 0);
    STORM_EXPECT_THROWS(std::invalid_argument, Storm::GetInt::random_range(0, 1, 0));
    STORM_EXPECT_THROWS(std::invalid_argument, Storm::GetInt::random_range(4, 4, 1));
}

void test_legacy_dice_and_symmetric_values() {
    Storm::Engine::seed(Storm::Unsigned{22});
    STORM_CHECK(Storm::GetInt::d(0) == 0);
    STORM_CHECK(Storm::GetInt::dice(0, 6) == 0);
    STORM_CHECK(Storm::GetInt::dice(3, 0) == 0);
    STORM_CHECK(Storm::GetInt::dice(integer_min, 0) == 0);
    for (int iteration = 0; iteration < 1024; ++iteration) {
        const Integer positive_die = Storm::GetInt::d(6);
        const Integer negative_die = Storm::GetInt::d(-6);
        const Integer positive_dice = Storm::GetInt::dice(3, 6);
        const Integer negative_sides = Storm::GetInt::dice(3, -6);
        const Integer negative_rolls = Storm::GetInt::dice(-3, 6);
        const Integer double_negative = Storm::GetInt::dice(-3, -6);
        STORM_CHECK(positive_die >= 1 && positive_die <= 6);
        STORM_CHECK(negative_die >= -6 && negative_die <= -1);
        STORM_CHECK(positive_dice >= 3 && positive_dice <= 18);
        STORM_CHECK(negative_sides >= -18 && negative_sides <= -3);
        STORM_CHECK(negative_rolls >= -18 && negative_rolls <= -3);
        STORM_CHECK(double_negative >= 3 && double_negative <= 18);

        const Integer flat = Storm::GetInt::plus_or_minus(10);
        const Integer linear = Storm::GetInt::plus_or_minus_linear(-10);
        const Integer gauss = Storm::GetInt::plus_or_minus_gauss(10);
        STORM_CHECK(flat >= -10 && flat <= 10);
        STORM_CHECK(linear >= -10 && linear <= 10);
        STORM_CHECK(gauss >= -10 && gauss <= 10);
    }

    const Integer extreme_die = Storm::GetInt::d(integer_min);
    STORM_CHECK(extreme_die >= integer_min && extreme_die <= -1);
    STORM_CHECK(Storm::GetInt::dice(integer_min, 1) == integer_min);
    const Integer extreme_single_roll = Storm::GetInt::dice(1, integer_min);
    STORM_CHECK(extreme_single_roll >= integer_min && extreme_single_roll <= -1);
    STORM_EXPECT_THROWS(std::overflow_error, Storm::GetInt::dice(integer_max, 2));
    STORM_EXPECT_THROWS(std::overflow_error, Storm::GetInt::dice(-integer_max, -2));
    STORM_EXPECT_THROWS(std::overflow_error, Storm::GetInt::dice(integer_min, 2));
    STORM_EXPECT_THROWS(std::overflow_error, Storm::GetInt::plus_or_minus(integer_min));
    STORM_EXPECT_THROWS(std::overflow_error, Storm::GetInt::plus_or_minus_linear(integer_min));
    STORM_EXPECT_THROWS(std::overflow_error, Storm::GetInt::plus_or_minus_gauss(integer_min));

    const Integer low_ability = Storm::GetInt::ability_dice(integer_min);
    const Integer high_ability = Storm::GetInt::ability_dice(integer_max);
    STORM_CHECK(low_ability >= 3 && low_ability <= 18);
    STORM_CHECK(high_ability >= 3 && high_ability <= 18);
}

void test_legacy_index_signs_and_boundaries() {
    Storm::Engine::seed(Storm::Unsigned{23});
    constexpr Integer beyond_exact_double = 9'007'199'254'740'992LL;

    for (const auto function : index_functions) {
        STORM_EXPECT_THROWS(std::invalid_argument, function(0));
        for (int iteration = 0; iteration < 128; ++iteration) {
            const Integer positive = function(17);
            const Integer negative = function(-17);
            STORM_CHECK(positive >= 0 && positive < 17);
            STORM_CHECK(negative >= -17 && negative < 0);
        }

        const Integer large_positive = function(beyond_exact_double);
        const Integer large_negative = function(-beyond_exact_double);
        const Integer minimum_negative = function(integer_min);
        STORM_CHECK(large_positive >= 0 && large_positive < beyond_exact_double);
        STORM_CHECK(large_negative >= -beyond_exact_double && large_negative < 0);
        STORM_CHECK(minimum_negative >= integer_min && minimum_negative < 0);
    }
}

}  // namespace

auto main() -> int {
    test_meters_and_gearbox();
    test_legacy_uniforms_and_range_phase();
    test_legacy_dice_and_symmetric_values();
    test_legacy_index_signs_and_boundaries();
    return storm_test::finish();
}

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
