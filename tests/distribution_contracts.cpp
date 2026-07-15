// SPDX-License-Identifier: MIT
#include <Storm/Storm.hpp>

#include "test_harness.hpp"

#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace {

constexpr double nan = std::numeric_limits<double>::quiet_NaN();
constexpr double infinity = std::numeric_limits<double>::infinity();

void test_boolean_domains() {
    Storm::seed(Storm::Unsigned{11});
    STORM_CHECK(!Storm::GetBool::bernoulli_variate(0.0));
    STORM_CHECK(Storm::GetBool::bernoulli_variate(1.0));
    STORM_CHECK(!Storm::GetBool::percent_true(0.0));
    STORM_CHECK(Storm::GetBool::percent_true(100.0));

    const Storm::engine_type state_before_invalid = Storm::thread_engine();
    for (const double invalid : {-0.01, 1.01, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetBool::bernoulli_variate(invalid));
    }
    for (const double invalid : {-0.01, 100.01, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetBool::percent_true(invalid));
    }
    STORM_CHECK(Storm::thread_engine() == state_before_invalid);
}

void test_integer_distribution_domains() {
    Storm::seed(Storm::Unsigned{12});
    STORM_CHECK(Storm::GetInt::binomial_variate(0, 0.5) == 0);
    STORM_CHECK(Storm::GetInt::binomial_variate(10, 0.0) == 0);
    STORM_CHECK(Storm::GetInt::binomial_variate(10, 1.0) == 10);
    STORM_CHECK(Storm::GetInt::negative_binomial_variate(5, 1.0) == 0);
    STORM_CHECK(Storm::GetInt::negative_binomial_variate(5, 0.5) >= 0);
    STORM_CHECK(Storm::GetInt::geometric_variate(0.5) >= 0);
    STORM_CHECK(Storm::GetInt::poisson_variate(2.0) >= 0);

    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetInt::binomial_variate(-1, 0.5));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetInt::negative_binomial_variate(0, 0.5));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetInt::poisson_variate(0.0));
    STORM_EXPECT_THROWS(std::overflow_error,
                        Storm::GetInt::poisson_variate(std::numeric_limits<double>::max()));

    for (const double invalid : {-0.01, 1.01, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetInt::binomial_variate(5, invalid));
    }
    for (const double invalid : {0.0, -0.01, 1.01, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetInt::negative_binomial_variate(5, invalid));
    }
    for (const double invalid : {0.0, 1.0, -0.01, 1.01, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetInt::geometric_variate(invalid));
    }
    for (const double invalid : {-1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetInt::poisson_variate(invalid));
    }
}

void test_basic_float_domains() {
    Storm::seed(Storm::Unsigned{13});
    for (int iteration = 0; iteration < 64; ++iteration) {
        const double uniform = Storm::GetFloat::uniform_real_variate(-2.0, 3.0);
        STORM_CHECK(uniform >= -2.0 && uniform < 3.0);
        STORM_CHECK(Storm::GetFloat::exponential_variate(2.0) >= 0.0);
        STORM_CHECK(Storm::GetFloat::gamma_variate(2.0, 3.0) >= 0.0);
        STORM_CHECK(Storm::GetFloat::weibull_variate(2.0, 3.0) >= 0.0);
        STORM_CHECK(std::isfinite(Storm::GetFloat::normal_variate(0.0, 1.0)));
        STORM_CHECK(Storm::GetFloat::log_normal_variate(0.0, 1.0) >= 0.0);
        STORM_CHECK(std::isfinite(Storm::GetFloat::extreme_value_variate(0.0, 1.0)));
        STORM_CHECK(Storm::GetFloat::chi_squared_variate(2.0) >= 0.0);
        STORM_CHECK(std::isfinite(Storm::GetFloat::cauchy_variate(0.0, 1.0)));
        STORM_CHECK(Storm::GetFloat::fisher_f_variate(2.0, 3.0) >= 0.0);
        STORM_CHECK(std::isfinite(Storm::GetFloat::student_t_variate(2.0)));
    }

    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetFloat::uniform_real_variate(1.0, 1.0));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetFloat::uniform_real_variate(2.0, 1.0));
    for (const double non_finite : {nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::uniform_real_variate(non_finite, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::uniform_real_variate(0.0, non_finite));
    }

    for (const double invalid : {0.0, -1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::exponential_variate(invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::chi_squared_variate(invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::student_t_variate(invalid));
    }

    for (const double invalid : {0.0, -1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::gamma_variate(invalid, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::gamma_variate(1.0, invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::weibull_variate(invalid, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::weibull_variate(1.0, invalid));
    }

    for (const double invalid : {0.0, -1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::normal_variate(0.0, invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::log_normal_variate(0.0, invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::extreme_value_variate(0.0, invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::cauchy_variate(0.0, invalid));
    }

    for (const double invalid_location : {nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::normal_variate(invalid_location, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::log_normal_variate(invalid_location, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::extreme_value_variate(invalid_location, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::cauchy_variate(invalid_location, 1.0));
    }

    for (const double invalid : {0.0, -1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::fisher_f_variate(invalid, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::fisher_f_variate(1.0, invalid));
    }
}

void test_custom_float_domains() {
    Storm::seed(Storm::Unsigned{14});
    for (int iteration = 0; iteration < 128; ++iteration) {
        const double beta = Storm::GetFloat::beta_variate(2.0, 3.0);
        const double pareto = Storm::GetFloat::pareto_variate(2.0);
        const double vonmises = Storm::GetFloat::vonmises_variate(1.0, 2.0);
        const double triangular = Storm::GetFloat::triangular_variate(-2.0, 4.0, 1.0);
        STORM_CHECK(beta >= 0.0 && beta <= 1.0);
        STORM_CHECK(pareto >= 1.0 && std::isfinite(pareto));
        STORM_CHECK(vonmises >= 0.0 && vonmises < 2.0 * std::numbers::pi);
        STORM_CHECK(triangular >= -2.0 && triangular <= 4.0);
    }

    const double huge_beta = Storm::GetFloat::beta_variate(
        std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    STORM_CHECK(huge_beta >= 0.0 && huge_beta <= 1.0);
    const double huge_location = Storm::GetFloat::vonmises_variate(
        std::numeric_limits<double>::max(), 2.0);
    STORM_CHECK(huge_location >= 0.0 && huge_location < 2.0 * std::numbers::pi);
    STORM_EXPECT_THROWS(
        std::overflow_error,
        Storm::GetFloat::vonmises_variate(0.0, std::numeric_limits<double>::max()));
    const double uniform_angle = Storm::GetFloat::vonmises_variate(0.0, 0.0);
    STORM_CHECK(uniform_angle >= 0.0 && uniform_angle < 2.0 * std::numbers::pi);
    STORM_CHECK(Storm::GetFloat::triangular_variate(0.0, 1.0, 0.0) >= 0.0);
    STORM_CHECK(Storm::GetFloat::triangular_variate(0.0, 1.0, 1.0) <= 1.0);

    for (const double invalid : {0.0, -1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::beta_variate(invalid, 1.0));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::beta_variate(1.0, invalid));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::pareto_variate(invalid));
    }

    for (const double invalid_mu : {nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::vonmises_variate(invalid_mu, 1.0));
    }
    for (const double invalid_kappa : {-1.0, nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::vonmises_variate(0.0, invalid_kappa));
    }

    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetFloat::triangular_variate(1.0, 1.0, 1.0));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetFloat::triangular_variate(2.0, 1.0, 1.5));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetFloat::triangular_variate(0.0, 1.0, -0.1));
    STORM_EXPECT_THROWS(std::invalid_argument,
                        Storm::GetFloat::triangular_variate(0.0, 1.0, 1.1));
    for (const double non_finite : {nan, infinity, -infinity}) {
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::triangular_variate(non_finite, 1.0, 0.5));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::triangular_variate(0.0, non_finite, 0.5));
        STORM_EXPECT_THROWS(std::invalid_argument,
                            Storm::GetFloat::triangular_variate(0.0, 1.0, non_finite));
    }
}

}  // namespace

auto main() -> int {
    test_boolean_domains();
    test_integer_distribution_domains();
    test_basic_float_domains();
    test_custom_float_domains();
    return storm_test::finish();
}

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
