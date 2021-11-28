#pragma once

#include <chrono>
#include <map>
#include <cstdio>
#include <string>
#include <utility>
#include "Utilities.hpp"


namespace MonkeyTimer {

    struct ScopeTimer {
        std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
        const std::string label;
        const size_t cycles;

        explicit ScopeTimer(std::string label, size_t cycles = 1) : label(std::move(label)), cycles(cycles) {
            start = std::chrono::high_resolution_clock::now();
        }

        ~ScopeTimer() {
            this -> end = std::chrono::high_resolution_clock::now();
            const std::chrono::nanoseconds duration{end - start};
            const auto nano{duration.count() / cycles};
            const auto sec{double(nano) / 1e+9};
            if (double(nano) < 1e+6) IO::print(label, ": ", nano, " nano\n");
            else IO::print(label, ": ", sec, " seconds\n");
        }
    };

    template<typename Function, typename... Args>
    auto function_timer(const std::string &label, Function &&func, const Args &... args) -> void {
        const auto cycles{1e+5};
        MonkeyTimer::ScopeTimer t(label, size_t(cycles));
        for (auto i{0}; i < cycles; ++i) {
            func(args...);
        }
    }

    template<typename Function, typename... Args>
    auto distribution_timer(const std::string &label, Function &&func, const Args &... args) -> void {
        const auto cycles{1e+5};
        MonkeyTimer::function_timer(label, func, args...);
        std::map<decltype(func(args...)), int64_t> history;
        for (auto i{0}; i < cycles; ++i) ++history[func(args...)];
        for (const auto&[key, val] : history) IO::print(key, ": ", val / (cycles / 100.0), "%\n");
        IO::print('\n');
    }

} // end namespace
