#pragma once

#include <chrono>
#include <map>
#include "Utilities.hpp"


namespace MonkeyTimer {

    struct ScopeTimer {
        std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
        const char *label;
        const size_t cycles;

        explicit ScopeTimer(const char *label, size_t cycles = 1) : label(label), cycles(cycles) {
            start = std::chrono::high_resolution_clock::now();
        }

        ~ScopeTimer() {
            this->end = std::chrono::high_resolution_clock::now();
            const std::chrono::nanoseconds duration{end - start};
            const size_t nano{duration.count() / cycles};
            if (nano < 1000000) {
                IO::print(label, ": ", nano, " nano\n");
            } else {
                const auto sec{nano / 1000000000.0l};
                IO::print(label, ": ", sec, " seconds\n");
            }
        }
    };

    template<typename Function, typename... Args>
    auto function_timer(const char *label, Function &&func, const Args &... args) -> void {
        const size_t cycles{100000};
        MonkeyTimer::ScopeTimer t(label, cycles);
        for (auto i{0}; i < cycles; ++i) {
            func(args...);
        }
    }

    template<typename Function, typename... Args>
    auto distribution_timer(const char *label, Function &&func, const Args &... args) -> void {
        const size_t cycles{100000};
        MonkeyTimer::function_timer(label, func, args...);
        std::map<decltype(func(args...)), int64_t> history;
        for (auto i{0}; i < cycles; ++i) ++history[func(args...)];
        for (const auto &[key, val]: history) IO::print(key, ": ", val / (cycles / 100.0), "%\n");
        IO::print('\n');
    }

}
