#pragma once

#include <chrono>
#include <map>
#include <utility>
#include "Utilities.hpp"

namespace MonkeyTimer {
    const auto version{"1.1.0"};

    struct ScopeTimer {
        std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
        const char* label;
        size_t cycles;

        explicit ScopeTimer(const char* label, size_t cycles = 1) : label(label), cycles(cycles) {
            start = std::chrono::high_resolution_clock::now();
        }

        ~ScopeTimer() {
            end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            double averageDuration = static_cast<double>(duration) / cycles;

            if (averageDuration < 1000000) {
                IO::print(label, ": ", averageDuration, " nano\n");
            } else {
                double sec = averageDuration / 1000000000.0;
                IO::print(label, ": ", sec, " seconds\n");
            }
        }
    };

    template<typename Function, typename... Args>
    auto function_timer(const char* label, Function&& func, Args&&... args) -> void {
        const size_t cycles = 100000;
        ScopeTimer t(label, cycles);
        for (size_t i = 0; i < cycles; ++i) {
            std::invoke(std::forward<Function>(func), std::forward<Args>(args)...);
        }
    }

    template<typename Function, typename... Args>
    auto distribution_timer(const char* label, Function&& func, Args&&... args) -> void {
        const size_t cycles = 100000;
        std::map<decltype(func(std::forward<Args>(args)...)), int64_t> history;
        function_timer(label, std::forward<Function>(func), std::forward<Args>(args)...);
        try {
            for (size_t i = 0; i < cycles; ++i) {
                auto result = std::invoke(std::forward<Function>(func), std::forward<Args>(args)...);
                ++history[result];
            }
        } catch (const std::exception& e) {
            IO::print("Exception caught during function execution: ", e.what(), "\n");
            return;
        }
        for (const auto& [key, val] : history) {
            IO::print(key, ": ", static_cast<double>(val) / cycles * 100.0, "%\n");
        }
        IO::print('\n');
    }
}
