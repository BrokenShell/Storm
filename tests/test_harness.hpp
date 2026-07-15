// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <exception>
#include <iostream>
#include <string_view>

namespace storm_test {

inline int failures = 0;

inline void check(const bool condition,
                  const std::string_view expression,
                  const std::string_view file,
                  const int line) {
    if (condition) {
        return;
    }
    ++failures;
    std::cerr << file << ':' << line << ": check failed: " << expression << '\n';
}

template <typename Exception, typename Callable>
void expect_throws(Callable&& callable,
                   const std::string_view expression,
                   const std::string_view file,
                   const int line) {
    try {
        callable();
    } catch (const Exception&) {
        return;
    } catch (const std::exception& error) {
        ++failures;
        std::cerr << file << ':' << line << ": " << expression
                  << " threw the wrong std::exception: " << error.what() << '\n';
        return;
    } catch (...) {
        ++failures;
        std::cerr << file << ':' << line << ": " << expression
                  << " threw a non-standard exception\n";
        return;
    }
    ++failures;
    std::cerr << file << ':' << line << ": expected exception from: " << expression << '\n';
}

inline auto finish() -> int {
    if (failures != 0) {
        std::cerr << failures << " test assertion(s) failed\n";
    }
    return failures == 0 ? 0 : 1;
}

inline auto approximately(const double actual, const double expected, const double tolerance) -> bool {
    return std::isfinite(actual) && std::fabs(actual - expected) <= tolerance;
}

}  // namespace storm_test

#define STORM_CHECK(expression) \
    ::storm_test::check(static_cast<bool>(expression), #expression, __FILE__, __LINE__)

#define STORM_EXPECT_THROWS(exception_type, expression)                                    \
    ::storm_test::expect_throws<exception_type>([&] { static_cast<void>(expression); },     \
                                                #expression, __FILE__, __LINE__)
