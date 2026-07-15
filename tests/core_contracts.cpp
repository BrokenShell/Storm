// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace {

using signed_type = std::int64_t;
using unsigned_type = std::uint64_t;

constexpr auto integer_min = std::numeric_limits<signed_type>::lowest();
constexpr auto integer_max = std::numeric_limits<signed_type>::max();
constexpr auto unsigned_max = std::numeric_limits<unsigned_type>::max();

void test_version_and_types() {
    static_assert(std::is_same_v<Storm::engine_type, std::mt19937_64>);
    static_assert(std::is_same_v<decltype(Storm::canonical()), double>);
    static_assert(std::is_same_v<decltype(Storm::uniform_integer(0, 1)), signed_type>);
    static_assert(std::is_same_v<decltype(Storm::uniform_unsigned(unsigned_type{0}, unsigned_type{1})),
                                 unsigned_type>);
    static_assert(std::is_same_v<decltype(Storm::uniform_index(std::size_t{1})), std::size_t>);
    static_assert(std::is_same_v<decltype(Storm::random_range(0, 1, 1)), signed_type>);
    static_assert(std::is_same_v<decltype(Storm::roll_die(std::size_t{1})), std::size_t>);
    static_assert(std::is_same_v<decltype(Storm::roll_dice(std::size_t{1}, std::size_t{1})),
                                 unsigned_type>);
    static_assert(std::is_same_v<decltype(Storm::ability_dice(std::size_t{3})), unsigned_type>);
    static_assert(!noexcept(Storm::uniform_integer(0, 1)));
    static_assert(!noexcept(Storm::uniform_unsigned(unsigned_type{0}, unsigned_type{1})));
    static_assert(!noexcept(Storm::uniform_index(std::size_t{1})));
    static_assert(!noexcept(Storm::random_range(0, 1, 1)));
    static_assert(!noexcept(Storm::roll_die(std::size_t{1})));
    static_assert(!noexcept(Storm::roll_dice(std::size_t{1}, std::size_t{1})));
    static_assert(!noexcept(Storm::ability_dice(std::size_t{3})));
    STORM_CHECK(std::string_view{Storm::version} == "5.0.0");
}

void test_owned_and_injected_engines() {
    Storm::Generator first{unsigned_type{0}};
    Storm::Generator second{unsigned_type{0}};
    static_assert(std::is_same_v<decltype(first.engine()), Storm::engine_type&>);

    for (int index = 0; index < 32; ++index) {
        STORM_CHECK(Storm::uniform_unsigned(first.engine(), unsigned_type{0}, unsigned_max) ==
                    Storm::uniform_unsigned(second.engine(), unsigned_type{0}, unsigned_max));
    }

    first.seed(unsigned_type{73});
    second.seed(unsigned_type{73});
    STORM_CHECK(Storm::uniform_integer(first.engine(), -100, 100) ==
                Storm::uniform_integer(second.engine(), -100, 100));

    Storm::Generator copied = first;
    STORM_CHECK(Storm::uniform_unsigned(first.engine(), unsigned_type{0}, unsigned_max) ==
                Storm::uniform_unsigned(copied.engine(), unsigned_type{0}, unsigned_max));

    static_assert(!noexcept(first.reseed_from_entropy()));
    static_assert(!noexcept(Storm::reseed_from_entropy()));
    static_assert(std::is_same_v<decltype(first.reseed_from_entropy()), void>);
    static_assert(std::is_same_v<decltype(Storm::reseed_from_entropy()), void>);
}

void test_tls_seed_zero() {
    std::array<unsigned_type, 16> first{};
    std::array<unsigned_type, 16> second{};
    Storm::seed(unsigned_type{0});
    for (auto& value : first) {
        value = Storm::uniform_unsigned(unsigned_type{0}, unsigned_max);
    }
    Storm::seed(unsigned_type{0});
    for (auto& value : second) {
        value = Storm::uniform_unsigned(unsigned_type{0}, unsigned_max);
    }
    STORM_CHECK(first == second);
    static_assert(std::is_same_v<decltype(Storm::thread_engine()), Storm::engine_type&>);
}

void test_v5_bounded_golden_vectors() {
    constexpr std::array<unsigned_type, 8> expected_unsigned{
        4ULL, 7ULL, 3ULL, 8ULL, 6ULL, 8ULL, 3ULL, 4ULL,
    };
    constexpr std::array<signed_type, 8> expected_integer{
        3LL, 0LL, -3LL, 1LL, -5LL, -1LL, -4LL, -1LL,
    };
    constexpr std::array<unsigned_type, 8> expected_full_unsigned{
        2'947'667'278'772'165'694ULL,
        18'301'848'765'998'365'067ULL,
        729'919'693'006'235'833ULL,
        11'021'831'128'136'023'278ULL,
        10'003'392'056'472'839'596ULL,
        1'054'412'044'467'431'918ULL,
        11'649'642'299'870'863'663ULL,
        7'813'497'161'378'842'344ULL,
    };
    constexpr std::array<signed_type, 8> expected_full_integer{
        -6'275'704'758'082'610'114LL,
        9'078'476'729'143'589'259LL,
        -8'493'452'343'848'539'975LL,
        1'798'459'091'281'247'470LL,
        780'020'019'618'063'788LL,
        -8'168'959'992'387'343'890LL,
        2'426'270'263'016'087'855LL,
        -1'409'874'875'475'933'464LL,
    };
    constexpr std::array<std::size_t, 8> expected_index{
        3U, 3U, 3U, 6U, 5U, 2U, 4U, 5U,
    };
    constexpr std::array<signed_type, 8> expected_range{
        0LL, 0LL, 0LL, 9LL, 6LL, -3LL, 3LL, 6LL,
    };
    constexpr std::array<std::size_t, 8> expected_die{
        1U, 6U, 2U, 1U, 5U, 3U, 2U, 1U,
    };

    Storm::Generator generator{unsigned_type{0}};
    for (const unsigned_type expected : expected_unsigned) {
        STORM_CHECK(Storm::uniform_unsigned(generator.engine(), unsigned_type{0}, unsigned_type{9}) ==
                    expected);
    }
    generator.seed(unsigned_type{0});
    for (const signed_type expected : expected_integer) {
        STORM_CHECK(Storm::uniform_integer(generator.engine(), -5, 5) == expected);
    }
    generator.seed(unsigned_type{0});
    for (const unsigned_type expected : expected_full_unsigned) {
        STORM_CHECK(Storm::uniform_unsigned(generator.engine(), unsigned_type{0}, unsigned_max) ==
                    expected);
    }
    generator.seed(unsigned_type{0});
    for (const signed_type expected : expected_full_integer) {
        STORM_CHECK(Storm::uniform_integer(generator.engine(), integer_min, integer_max) == expected);
    }
    generator.seed(unsigned_type{0});
    for (const std::size_t expected : expected_index) {
        STORM_CHECK(Storm::uniform_index(generator.engine(), std::size_t{7}) == expected);
    }
    generator.seed(unsigned_type{0});
    for (const signed_type expected : expected_range) {
        STORM_CHECK(Storm::random_range(generator.engine(), -9, 10, 3) == expected);
    }
    generator.seed(unsigned_type{0});
    for (const std::size_t expected : expected_die) {
        STORM_CHECK(Storm::roll_die(generator.engine(), std::size_t{6}) == expected);
    }
}

void test_canonical() {
    Storm::Generator generator{unsigned_type{19}};
    for (int index = 0; index < 2048; ++index) {
        const double value = Storm::canonical(generator.engine());
        STORM_CHECK(value >= 0.0);
        STORM_CHECK(value < 1.0);
    }
    Storm::seed(unsigned_type{19});
    const double tls_value = Storm::canonical();
    STORM_CHECK(tls_value >= 0.0 && tls_value < 1.0);
}

void test_uniform_integers() {
    Storm::Generator generator{unsigned_type{101}};
    STORM_CHECK(Storm::uniform_integer(generator.engine(), integer_min, integer_min) == integer_min);
    STORM_CHECK(Storm::uniform_integer(generator.engine(), integer_max, integer_max) == integer_max);
    STORM_CHECK(Storm::uniform_unsigned(generator.engine(), unsigned_type{0}, unsigned_type{0}) == unsigned_type{0});
    STORM_CHECK(Storm::uniform_unsigned(generator.engine(), unsigned_max, unsigned_max) == unsigned_max);

    for (int index = 0; index < 4096; ++index) {
        const signed_type signed_value = Storm::uniform_integer(generator.engine(), integer_min, integer_max);
        const unsigned_type unsigned_value =
            Storm::uniform_unsigned(generator.engine(), unsigned_type{0}, unsigned_max);
        STORM_CHECK(signed_value >= integer_min && signed_value <= integer_max);
        STORM_CHECK(unsigned_value <= unsigned_max);
    }

    const Storm::engine_type state_before_invalid = generator.engine();
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::uniform_integer(generator.engine(), 1, -1));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::uniform_unsigned(generator.engine(), unsigned_type{1}, unsigned_type{0}));
    STORM_CHECK(generator.engine() == state_before_invalid);
}

void test_uniform_index() {
    Storm::Generator generator{unsigned_type{202}};
    STORM_CHECK(Storm::uniform_index(generator.engine(), std::size_t{1}) == 0U);
    for (int index = 0; index < 2048; ++index) {
        STORM_CHECK(Storm::uniform_index(generator.engine(), std::size_t{17}) < 17U);
    }
    STORM_CHECK(Storm::uniform_index(generator.engine(), std::numeric_limits<std::size_t>::max()) <
                std::numeric_limits<std::size_t>::max());
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::uniform_index(generator.engine(), std::size_t{0}));
    STORM_EXPECT_THROWS(std::invalid_argument, Storm::uniform_index(std::size_t{0}));
    Storm::seed(unsigned_type{202});
    STORM_CHECK(Storm::uniform_index(std::size_t{17}) < 17U);
}

void test_directed_ranges() {
    Storm::Generator generator{unsigned_type{303}};
    for (int index = 0; index < 2048; ++index) {
        const signed_type ascending = Storm::random_range(generator.engine(), 1, 10, 2);
        STORM_CHECK(ascending >= 1 && ascending < 10);
        STORM_CHECK((ascending - 1) % 2 == 0);

        const signed_type descending = Storm::random_range(generator.engine(), 10, 1, -2);
        STORM_CHECK(descending <= 10 && descending > 1);
        STORM_CHECK((10 - descending) % 2 == 0);
    }

    for (int index = 0; index < 256; ++index) {
        const signed_type wide = Storm::random_range(generator.engine(), integer_min, integer_max, integer_max);
        STORM_CHECK(wide == integer_min || wide == -1 || wide == integer_max - 1);

        const signed_type huge_step = Storm::random_range(generator.engine(), integer_max, integer_min, integer_min);
        STORM_CHECK(huge_step == integer_max || huge_step == -1);
    }

    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::random_range(generator.engine(), 0, 10, 0));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::random_range(generator.engine(), 4, 4, 1));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::random_range(generator.engine(), 0, 10, -1));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::random_range(generator.engine(), 10, 0, 1));
    Storm::seed(unsigned_type{303});
    const signed_type tls_value = Storm::random_range(1, 10, 2);
    STORM_CHECK(tls_value >= 1 && tls_value < 10 && (tls_value - 1) % 2 == 0);
}

void test_dice() {
    Storm::Generator generator{unsigned_type{404}};
    STORM_CHECK(Storm::roll_die(generator.engine(), std::size_t{1}) == std::size_t{1});
    STORM_CHECK(Storm::roll_dice(generator.engine(), std::size_t{0}, std::size_t{6}) ==
                unsigned_type{0});
    STORM_CHECK(Storm::roll_dice(generator.engine(),
                                 std::numeric_limits<std::size_t>::max(),
                                 std::size_t{1}) ==
                static_cast<unsigned_type>(std::numeric_limits<std::size_t>::max()));
    const std::size_t maximum_die =
        Storm::roll_die(generator.engine(), std::numeric_limits<std::size_t>::max());
    STORM_CHECK(maximum_die >= 1U);
    const unsigned_type maximum_single_roll =
        Storm::roll_dice(generator.engine(), std::size_t{1},
                         std::numeric_limits<std::size_t>::max());
    STORM_CHECK(maximum_single_roll >= unsigned_type{1});
    for (int index = 0; index < 1024; ++index) {
        const std::size_t die = Storm::roll_die(generator.engine(), std::size_t{20});
        const unsigned_type dice = Storm::roll_dice(generator.engine(), std::size_t{3}, std::size_t{6});
        const unsigned_type ability = Storm::ability_dice(generator.engine(), std::size_t{10});
        STORM_CHECK(die >= 1U && die <= 20U);
        STORM_CHECK(dice >= 3U && dice <= 18U);
        STORM_CHECK(ability >= 3U && ability <= 18U);
    }
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::roll_die(generator.engine(), std::size_t{0}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::roll_dice(generator.engine(), std::size_t{1}, std::size_t{0}));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::ability_dice(generator.engine(), std::size_t{2}));
    STORM_CHECK(Storm::ability_dice(
                    generator.engine(), std::numeric_limits<std::size_t>::max()) ==
                unsigned_type{18});
    STORM_EXPECT_THROWS(std::overflow_error,
                        Storm::roll_dice(generator.engine(),
                                         std::numeric_limits<std::size_t>::max(),
                                         std::size_t{2}));
    Storm::seed(unsigned_type{404});
    STORM_CHECK(Storm::roll_die(std::size_t{6}) >= 1U);
    STORM_CHECK(Storm::roll_dice(std::size_t{3}, std::size_t{6}) >= 3U);
    STORM_CHECK(Storm::ability_dice(std::size_t{4}) >= 3U);
}

}  // namespace

auto main() -> int {
    test_version_and_types();
    test_owned_and_injected_engines();
    test_tls_seed_zero();
    test_v5_bounded_golden_vectors();
    test_canonical();
    test_uniform_integers();
    test_uniform_index();
    test_directed_ranges();
    test_dice();
    return storm_test::finish();
}
