#include "Storm.hpp"

extern "C" {
    const char* get_version() {
        return Storm::get_version();
    }

    Storm::UnsignedInteger max_uint() {
        return Storm::Meters::max_uint();
    }

    Storm::Integer min_int() {
        return Storm::Meters::min_int();
    }

    Storm::Integer max_int() {
        return Storm::Meters::max_int();
    }

    Storm::Float min_float() {
        return Storm::Meters::min_float();
    }

    Storm::Float max_float() {
        return Storm::Meters::max_float();
    }

    Storm::Float min_below() {
        return Storm::Meters::min_below();
    }

    Storm::Float min_above() {
        return Storm::Meters::min_above();
    }

    Storm::Float canonical_variate() {
        return Storm::GetFloat::canonical_variate();
    }

    Storm::Float uniform_real_variate(Storm::Float a, Storm::Float b) {
        return Storm::GetFloat::uniform_real_variate(a, b);
    }

    Storm::Float exponential_variate(Storm::Float lambda_rate) {
        return Storm::GetFloat::exponential_variate(lambda_rate);
    }

    Storm::Float gamma_variate(Storm::Float shape, Storm::Float scale) {
        return Storm::GetFloat::gamma_variate(shape, scale);
    }

    Storm::Float weibull_variate(Storm::Float shape, Storm::Float scale) {
        return Storm::GetFloat::weibull_variate(shape, scale);
    }

    Storm::Float normal_variate(Storm::Float mean, Storm::Float std_dev) {
        return Storm::GetFloat::normal_variate(mean, std_dev);
    }

    Storm::Float log_normal_variate(Storm::Float log_mean, Storm::Float log_deviation) {
        return Storm::GetFloat::log_normal_variate(log_mean, log_deviation);
    }

    Storm::Float extreme_value_variate(Storm::Float location, Storm::Float scale) {
        return Storm::GetFloat::extreme_value_variate(location, scale);
    }

    Storm::Float chi_squared_variate(double degrees_of_freedom) {
        return Storm::GetFloat::chi_squared_variate(degrees_of_freedom);
    }

    Storm::Float cauchy_variate(Storm::Float location, Storm::Float scale) {
        return Storm::GetFloat::cauchy_variate(location, scale);
    }

    Storm::Float fisher_f_variate(double degrees_of_freedom_1, double degrees_of_freedom_2) {
        return Storm::GetFloat::fisher_f_variate(degrees_of_freedom_1, degrees_of_freedom_2);
    }

    Storm::Float student_t_variate(double degrees_of_freedom) {
        return Storm::GetFloat::student_t_variate(degrees_of_freedom);
    }

    Storm::Float beta_variate(Storm::Float alpha, Storm::Float beta) {
        return Storm::GetFloat::beta_variate(alpha, beta);
    }

    Storm::Float pareto_variate(Storm::Float alpha) {
        return Storm::GetFloat::pareto_variate(alpha);
    }

    Storm::Float vonmises_variate(Storm::Float mu, Storm::Float kappa) {
        return Storm::GetFloat::vonmises_variate(mu, kappa);
    }

    Storm::Float triangular_variate(Storm::Float low, Storm::Float high, Storm::Float mode) {
        return Storm::GetFloat::triangular_variate(low, high, mode);
    }

    bool bernoulli_variate(double truth_factor) {
        return Storm::GetBool::bernoulli_variate(truth_factor);
    }

    bool percent_true(Storm::Float truth_factor) {
        return Storm::GetBool::percent_true(truth_factor);
    }

    Storm::UnsignedInteger uniform_uint_variate(Storm::UnsignedInteger lo, Storm::UnsignedInteger hi) {
        return Storm::GetInt::uniform_uint_variate(lo, hi);
    }

    Storm::Integer uniform_int_variate(Storm::Integer lo, Storm::Integer hi) {
        return Storm::GetInt::uniform_int_variate(lo, hi);
    }

    Storm::Integer binomial_variate(Storm::Integer number_of_trials, double probability) {
        return Storm::GetInt::binomial_variate(number_of_trials, probability);
    }

    Storm::Integer negative_binomial_variate(Storm::Integer number_of_trials, double probability) {
        return Storm::GetInt::negative_binomial_variate(number_of_trials, probability);
    }

    Storm::Integer geometric_variate(double probability) {
        return Storm::GetInt::geometric_variate(probability);
    }

    Storm::Integer poisson_variate(double mean) {
        return Storm::GetInt::poisson_variate(mean);
    }

    Storm::Integer random_below(Storm::Integer number) {
        return Storm::GetInt::random_below(number);
    }

    Storm::Integer random_range(Storm::Integer start, Storm::Integer stop, Storm::Integer step) {
        return Storm::GetInt::random_range(start, stop, step);
    }

    Storm::Integer d(Storm::Integer sides) {
        return Storm::GetInt::d(sides);
    }

    Storm::Integer dice(Storm::Integer rolls, Storm::Integer sides) {
        return Storm::GetInt::dice(rolls, sides);
    }

    Storm::Integer ability_dice(Storm::Integer number) {
        return Storm::GetInt::ability_dice(number);
    }

    Storm::Integer plus_or_minus(Storm::Integer number) {
        return Storm::GetInt::plus_or_minus(number);
    }

    Storm::Integer plus_or_minus_linear(Storm::Integer number) {
        return Storm::GetInt::plus_or_minus_linear(number);
    }

    Storm::Integer plus_or_minus_gauss(Storm::Integer number) {
        return Storm::GetInt::plus_or_minus_gauss(number);
    }

    Storm::Integer random_index(Storm::Integer number) {
        return Storm::GetIndex::random_index(number);
    }

    Storm::Integer front_linear(Storm::Integer number) {
        return Storm::GetIndex::front_linear(number);
    }

    Storm::Integer back_linear(Storm::Integer number) {
        return Storm::GetIndex::back_linear(number);
    }

    Storm::Integer middle_linear(Storm::Integer number) {
        return Storm::GetIndex::middle_linear(number);
    }

    Storm::Integer quantum_linear(Storm::Integer number) {
        return Storm::GetIndex::quantum_linear(number);
    }

    Storm::Integer front_gauss(Storm::Integer number) {
        return Storm::GetIndex::front_gauss(number);
    }

    Storm::Integer middle_gauss(Storm::Integer number) {
        return Storm::GetIndex::middle_gauss(number);
    }

    Storm::Integer back_gauss(Storm::Integer number) {
        return Storm::GetIndex::back_gauss(number);
    }

    Storm::Integer quantum_gauss(Storm::Integer number) {
        return Storm::GetIndex::quantum_gauss(number);
    }

    Storm::Integer front_poisson(Storm::Integer number) {
        return Storm::GetIndex::front_poisson(number);
    }

    Storm::Integer back_poisson(Storm::Integer number) {
        return Storm::GetIndex::back_poisson(number);
    }

    Storm::Integer middle_poisson(Storm::Integer number) {
        return Storm::GetIndex::middle_poisson(number);
    }

    Storm::Integer quantum_poisson(Storm::Integer number) {
        return Storm::GetIndex::quantum_poisson(number);
    }

    Storm::Integer quantum_monty(Storm::Integer number) {
        return Storm::GetIndex::quantum_monty(number);
    }
}
