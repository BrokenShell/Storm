// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <array>
#include <cstdint>
#include <thread>

namespace {

using Sequence = std::array<std::int64_t, 8>;

auto draw_tls_sequence(const std::uint64_t seed_value) -> Sequence {
    Storm::seed(seed_value);
    Sequence sequence{};
    for (auto& value : sequence) {
        value = Storm::uniform_integer(-1'000'000, 1'000'000);
    }
    return sequence;
}

auto draw_owned_sequence(const std::uint64_t seed_value) -> Sequence {
    Storm::Generator generator{seed_value};
    Sequence sequence{};
    for (auto& value : sequence) {
        value = Storm::uniform_integer(generator.engine(), -1'000'000, 1'000'000);
    }
    return sequence;
}

}  // namespace

auto main() -> int {
    const Sequence expected = draw_owned_sequence(8128);
    Sequence first_thread{};
    Sequence second_thread{};

    std::thread first{[&] { first_thread = draw_tls_sequence(8128); }};
    std::thread second{[&] { second_thread = draw_tls_sequence(8128); }};
    first.join();
    second.join();

    STORM_CHECK(first_thread == expected);
    STORM_CHECK(second_thread == expected);

    Storm::seed(std::uint64_t{77});
    const auto main_first = Storm::uniform_integer(-1000, 1000);
    std::thread unrelated{[] {
        Storm::seed(std::uint64_t{999});
        for (int index = 0; index < 100; ++index) {
            static_cast<void>(Storm::uniform_integer(-1000, 1000));
        }
    }};
    unrelated.join();
    const auto main_second = Storm::uniform_integer(-1000, 1000);

    Storm::Generator control{std::uint64_t{77}};
    STORM_CHECK(main_first == Storm::uniform_integer(control.engine(), -1000, 1000));
    STORM_CHECK(main_second == Storm::uniform_integer(control.engine(), -1000, 1000));
    return storm_test::finish();
}
