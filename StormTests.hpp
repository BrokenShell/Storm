#pragma once

#include <iomanip>
#include "Storm.hpp"
#include "MonkeyTimer.hpp"

namespace Tests {

auto integer_tests() -> void {
  MonkeyTimer::print("Random Integer \n--------------\n\n");
  MonkeyTimer::distribution_timer("random_below(10)", Storm::random_below, 10);
  MonkeyTimer::distribution_timer("uniform_int_variate(0, 9)", Storm::uniform_int_variate, 0, 9);
  MonkeyTimer::distribution_timer("random_range(0, 20, 2)", Storm::random_range, 0, 20, 2);
  MonkeyTimer::distribution_timer("random_range(0, 20, -2)", Storm::random_range, 0, 20, -2);
  MonkeyTimer::distribution_timer("random_range(0, -20, 2)", Storm::random_range, 0, -20, 2);
  MonkeyTimer::distribution_timer("random_range(0, -20, -2)", Storm::random_range, 0, -20, -2);
  MonkeyTimer::distribution_timer("plus_or_minus(10)", Storm::plus_or_minus, 10);
  MonkeyTimer::distribution_timer("plus_or_minus_linear(10)", Storm::plus_or_minus_linear, 10);
  MonkeyTimer::distribution_timer("plus_or_minus_gauss(10)", Storm::plus_or_minus_gauss, 10);
  MonkeyTimer::distribution_timer("binomial_variate(10, 0.5)", Storm::binomial_variate, 10, 0.5);
  MonkeyTimer::distribution_timer("negative_binomial_variate(5, 0.75)", Storm::negative_binomial_variate, 5,
                                  0.75);
  MonkeyTimer::distribution_timer("geometric_variate(0.75)", Storm::geometric_variate, 0.75);
  MonkeyTimer::distribution_timer("poisson_variate(4.0)", Storm::poisson_variate, 4.0);
  MonkeyTimer::print("\n");
  MonkeyTimer::print("Random Dice \n-----------\n\n");
  MonkeyTimer::distribution_timer("d(10)", Storm::d, 10);
  MonkeyTimer::distribution_timer("dice(3, 6)", Storm::dice, 3, 6);
  MonkeyTimer::distribution_timer("ability_dice(4)", Storm::ability_dice, 4);
  MonkeyTimer::print("\n");
}

auto bool_tests() -> void {
  MonkeyTimer::print("Random Boolean \n--------------\n\n");
  MonkeyTimer::distribution_timer("percent_true(25.0)", Storm::percent_true, 25.0);
  MonkeyTimer::distribution_timer("bernoulli_variate(0.25)", Storm::bernoulli_variate, 0.25);
  MonkeyTimer::print("\n");
}

auto float_tests() -> void {
  MonkeyTimer::print("Random Float: timer only \n------------------------\n\n");
  MonkeyTimer::function_timer("canonical_variate()", Storm::canonical_variate);
  MonkeyTimer::function_timer("uniform_real_variate(1.0, 10.0)", Storm::uniform_real_variate, 1.0, 10.0);
  MonkeyTimer::function_timer("exponential_variate(1.0)", Storm::exponential_variate, 1.0);
  MonkeyTimer::function_timer("gamma_variate(1.0, 1.0)", Storm::gamma_variate, 1.0, 1.0);
  MonkeyTimer::function_timer("weibull_variate(1.0, 1.0)", Storm::weibull_variate, 1.0, 1.0);
  MonkeyTimer::function_timer("normal_variate(5, 2)", Storm::normal_variate, 5, 2);
  MonkeyTimer::function_timer("lognormal_variate(1.6, 0.25)", Storm::lognormal_variate, 1.6, 0.25);
  MonkeyTimer::function_timer("extreme_value_variate(0.0, 1.0)", Storm::extreme_value_variate, 0.0, 1.0);
  MonkeyTimer::function_timer("chi_squared_variate(1.0)", Storm::chi_squared_variate, 1.0);
  MonkeyTimer::function_timer("cauchy_variate(0.0, 0.00005)", Storm::cauchy_variate, 0.0, 0.00005);
  MonkeyTimer::function_timer("fisher_f_variate(1.0, 1.0)", Storm::fisher_f_variate, 1.0, 1.0);
  MonkeyTimer::function_timer("student_t_variate(1.0)", Storm::student_t_variate, 1.0);
  MonkeyTimer::function_timer("beta_variate(10.0, 1.0)", Storm::beta_variate, 10.0, 1.0);
  MonkeyTimer::function_timer("pareto_variate(5.0)", Storm::pareto_variate, 5.0);
  MonkeyTimer::function_timer("vonmises_variate(1.0, 1.0)", Storm::vonmises_variate, 1.0, 1.0);
  MonkeyTimer::function_timer("triangular_variate(0.0, 10.0, 5.0)", Storm::triangular_variate, 0.0, 10.0, 5.0);
  MonkeyTimer::print("\n\n");
}

auto random_index_tests() -> void {
  MonkeyTimer::print("Random Index: ZeroCool \n----------------------\n\n");
  const int N{10};
  MonkeyTimer::print("F(N) where N = ", N, "\n\n");
  MonkeyTimer::distribution_timer("random_index(N)", Storm::random_index, N);
  MonkeyTimer::distribution_timer("front_linear(N)", Storm::front_linear, N);
  MonkeyTimer::distribution_timer("middle_linear(N)", Storm::middle_linear, N);
  MonkeyTimer::distribution_timer("back_linear(N)", Storm::back_linear, N);
  MonkeyTimer::distribution_timer("quantum_linear(N)", Storm::quantum_linear, N);
  MonkeyTimer::distribution_timer("front_gauss(N)", Storm::front_gauss, N);
  MonkeyTimer::distribution_timer("middle_gauss(N)", Storm::middle_gauss, N);
  MonkeyTimer::distribution_timer("back_gauss(N)", Storm::back_gauss, N);
  MonkeyTimer::distribution_timer("quantum_gauss(N)", Storm::quantum_gauss, N);
  MonkeyTimer::distribution_timer("front_poisson(N)", Storm::front_poisson, N);
  MonkeyTimer::distribution_timer("middle_poisson(N)", Storm::middle_poisson, N);
  MonkeyTimer::distribution_timer("back_poisson(N)", Storm::back_poisson, N);
  MonkeyTimer::distribution_timer("quantum_poisson(N)", Storm::quantum_poisson, N);
  MonkeyTimer::distribution_timer("quantum_monty(N)", Storm::quantum_monty, N);
  MonkeyTimer::print("F(-N) where N = ", N, "\n\n");
  MonkeyTimer::distribution_timer("random_index(-N)", Storm::random_index, -N);
  MonkeyTimer::distribution_timer("front_linear(-N)", Storm::front_linear, -N);
  MonkeyTimer::distribution_timer("middle_linear(-N)", Storm::middle_linear, -N);
  MonkeyTimer::distribution_timer("back_linear(-N)", Storm::back_linear, -N);
  MonkeyTimer::distribution_timer("quantum_linear(-N)", Storm::quantum_linear, -N);
  MonkeyTimer::distribution_timer("front_gauss(-N)", Storm::front_gauss, -N);
  MonkeyTimer::distribution_timer("middle_gauss(-N)", Storm::middle_gauss, -N);
  MonkeyTimer::distribution_timer("back_gauss(-N)", Storm::back_gauss, -N);
  MonkeyTimer::distribution_timer("quantum_gauss(-N)", Storm::quantum_gauss, -N);
  MonkeyTimer::distribution_timer("front_poisson(-N)", Storm::front_poisson, -N);
  MonkeyTimer::distribution_timer("middle_poisson(-N)", Storm::middle_poisson, -N);
  MonkeyTimer::distribution_timer("back_poisson(-N)", Storm::back_poisson, -N);
  MonkeyTimer::distribution_timer("quantum_poisson(-N)", Storm::quantum_poisson, -N);
  MonkeyTimer::distribution_timer("quantum_monty(-N)", Storm::quantum_monty, -N);
  MonkeyTimer::print("\n");
}

auto min_max_tests() -> void {
  MonkeyTimer::print("Min/Max Tests \n-------------\n\n");
  MonkeyTimer::print("Minimum Integer: ", Storm::Meters::min_int(), '\n');
  MonkeyTimer::print("Maximum Integer:  ", Storm::Meters::max_int(), '\n');
  MonkeyTimer::print("Minimum Float: ", Storm::Meters::min_float(), '\n');
  MonkeyTimer::print("Maximum Float:  ", Storm::Meters::max_float(), '\n');
  MonkeyTimer::print("Minimum Below: ", Storm::Meters::min_below(), '\n');
  MonkeyTimer::print("Minimum Above:  ", Storm::Meters::min_above(), '\n');
  MonkeyTimer::print("\n\n");
}

auto clamp_tests() -> void {
  MonkeyTimer::print("Clamp Tests \n-----------\n\n");
  MonkeyTimer::function_timer("clamp(150, 1, 100)", Storm::GearBox::clamp<int>, 150, 1, 100);
  MonkeyTimer::function_timer("smart_clamp(150, 1, 100)", Storm::GearBox::smart_clamp, 150, 1, 100);
  MonkeyTimer::print("\n\n");
}

auto run_tests() -> void {
  {
    MonkeyTimer::ScopeTimer t("Total Time");
    MonkeyTimer::print("\nMonkeyTimer\nStorm 3.4.2\n");
    MonkeyTimer::print("===========\n\n\n");
    Tests::min_max_tests();
    Tests::bool_tests();
    Tests::integer_tests();
    Tests::random_index_tests();
    Tests::float_tests();
    Tests::clamp_tests();
    MonkeyTimer::print("============================\n");
  }
  MonkeyTimer::print("\n");
}

} // end namespace
