#pragma once

#include "Storm.hpp"
#include "MonkeyTimer.hpp"
#include "Utilities.hpp"

namespace StormTests {
    auto integer_tests() -> void {
        IO::print("Random Integer \n--------------\n");
        MonkeyTimer::distribution_timer("random_below(10)", Storm::GetInt::random_below, 10);
        MonkeyTimer::distribution_timer("uniform_int_variate(0, 9)", Storm::GetInt::uniform_int_variate, 0, 9);
        MonkeyTimer::distribution_timer("random_range(0, 20, 2)", Storm::GetInt::random_range, 0, 20, 2);
        MonkeyTimer::distribution_timer("random_range(0, 20, -2)", Storm::GetInt::random_range, 0, 20, -2);
        MonkeyTimer::distribution_timer("random_range(0, -20, 2)", Storm::GetInt::random_range, 0, -20, 2);
        MonkeyTimer::distribution_timer("random_range(0, -20, -2)", Storm::GetInt::random_range, 0, -20, -2);
        MonkeyTimer::distribution_timer("plus_or_minus(10)", Storm::GetInt::plus_or_minus, 10);
        MonkeyTimer::distribution_timer("plus_or_minus_linear(10)", Storm::GetInt::plus_or_minus_linear, 10);
        MonkeyTimer::distribution_timer("plus_or_minus_gauss(10)", Storm::GetInt::plus_or_minus_gauss, 10);
        MonkeyTimer::distribution_timer("binomial_variate(10, 0.5)", Storm::GetInt::binomial_variate, 10, 0.5);
        MonkeyTimer::distribution_timer("negative_binomial_variate(5, 0.75)",
                                        Storm::GetInt::negative_binomial_variate, 5, 0.75);
        MonkeyTimer::distribution_timer("geometric_variate(0.75)", Storm::GetInt::geometric_variate, 0.75);
        MonkeyTimer::distribution_timer("poisson_variate(4.0)", Storm::GetInt::poisson_variate, 4.0);
        IO::print("Random Dice \n-----------\n");
        MonkeyTimer::distribution_timer("d(10)", Storm::GetInt::d, 10);
        MonkeyTimer::distribution_timer("dice(3, 6)", Storm::GetInt::dice, 3, 6);
        MonkeyTimer::distribution_timer("dice(0, 6)", Storm::GetInt::dice, 0, 6);
        MonkeyTimer::distribution_timer("ability_dice(4)", Storm::GetInt::ability_dice, 4);
    }

    auto bool_tests() -> void {
        IO::print("Random Boolean \n--------------\n");
        MonkeyTimer::distribution_timer("percent_true(25.0)", Storm::GetBool::percent_true, 25.0);
        MonkeyTimer::distribution_timer("bernoulli_variate(0.25)", Storm::GetBool::bernoulli_variate, 0.25);
    }

    auto float_tests() -> void {
        IO::print("Random Float: timer only \n------------------------\n");
        MonkeyTimer::function_timer("canonical_variate()", Storm::GetFloat::canonical_variate);
        MonkeyTimer::function_timer("uniform_real_variate(1.0, 10.0)",
                                    Storm::GetFloat::uniform_real_variate, 1.0, 10.0);
        MonkeyTimer::function_timer("exponential_variate(1.0)", Storm::GetFloat::exponential_variate, 1.0);
        MonkeyTimer::function_timer("gamma_variate(1.0, 1.0)", Storm::GetFloat::gamma_variate, 1.0, 1.0);
        MonkeyTimer::function_timer("weibull_variate(1.0, 1.0)", Storm::GetFloat::weibull_variate, 1.0, 1.0);
        MonkeyTimer::function_timer("normal_variate(5, 2)", Storm::GetFloat::normal_variate, 5, 2);
        MonkeyTimer::function_timer("lognormal_variate(1.6, 0.25)", Storm::GetFloat::lognormal_variate, 1.6, 0.25);
        MonkeyTimer::function_timer("extreme_value_variate(0.0, 1.0)",
                                    Storm::GetFloat::extreme_value_variate, 0.0, 1.0);
        MonkeyTimer::function_timer("chi_squared_variate(1.0)", Storm::GetFloat::chi_squared_variate, 1.0);
        MonkeyTimer::function_timer("cauchy_variate(0.0, 0.00005)", Storm::GetFloat::cauchy_variate, 0.0, 0.00005);
        MonkeyTimer::function_timer("fisher_f_variate(1.0, 1.0)", Storm::GetFloat::fisher_f_variate, 1.0, 1.0);
        MonkeyTimer::function_timer("student_t_variate(1.0)", Storm::GetFloat::student_t_variate, 1.0);
        MonkeyTimer::function_timer("beta_variate(10.0, 1.0)", Storm::GetFloat::beta_variate, 10.0, 1.0);
        MonkeyTimer::function_timer("pareto_variate(5.0)", Storm::GetFloat::pareto_variate, 5.0);
        MonkeyTimer::function_timer("vonmises_variate(1.0, 1.0)", Storm::GetFloat::vonmises_variate, 1.0, 1.0);
        MonkeyTimer::function_timer("triangular_variate(0.0, 10.0, 5.0)",
                                    Storm::GetFloat::triangular_variate, 0.0, 10.0, 5.0);
        IO::print("\n");
    }

    auto random_index_tests() -> void {
        IO::print("Random Index: ZeroCool \n----------------------\n");
        const auto N{10};
        IO::print("F(N) where N = ", N, "\n");
        MonkeyTimer::distribution_timer("random_index(N)", Storm::GetIndex::random_index, N);
        MonkeyTimer::distribution_timer("front_linear(N)", Storm::GetIndex::front_linear, N);
        MonkeyTimer::distribution_timer("middle_linear(N)", Storm::GetIndex::middle_linear, N);
        MonkeyTimer::distribution_timer("back_linear(N)", Storm::GetIndex::back_linear, N);
        MonkeyTimer::distribution_timer("quantum_linear(N)", Storm::GetIndex::quantum_linear, N);
        MonkeyTimer::distribution_timer("front_gauss(N)", Storm::GetIndex::front_gauss, N);
        MonkeyTimer::distribution_timer("middle_gauss(N)", Storm::GetIndex::middle_gauss, N);
        MonkeyTimer::distribution_timer("back_gauss(N)", Storm::GetIndex::back_gauss, N);
        MonkeyTimer::distribution_timer("quantum_gauss(N)", Storm::GetIndex::quantum_gauss, N);
        MonkeyTimer::distribution_timer("front_poisson(N)", Storm::GetIndex::front_poisson, N);
        MonkeyTimer::distribution_timer("middle_poisson(N)", Storm::GetIndex::middle_poisson, N);
        MonkeyTimer::distribution_timer("back_poisson(N)", Storm::GetIndex::back_poisson, N);
        MonkeyTimer::distribution_timer("quantum_poisson(N)", Storm::GetIndex::quantum_poisson, N);
        MonkeyTimer::distribution_timer("quantum_monty(N)", Storm::GetIndex::quantum_monty, N);
        IO::print("F(-N) where N = ", N, "\n");
        MonkeyTimer::distribution_timer("random_index(-N)", Storm::GetIndex::random_index, -N);
        MonkeyTimer::distribution_timer("front_linear(-N)", Storm::GetIndex::front_linear, -N);
        MonkeyTimer::distribution_timer("middle_linear(-N)", Storm::GetIndex::middle_linear, -N);
        MonkeyTimer::distribution_timer("back_linear(-N)", Storm::GetIndex::back_linear, -N);
        MonkeyTimer::distribution_timer("quantum_linear(-N)", Storm::GetIndex::quantum_linear, -N);
        MonkeyTimer::distribution_timer("front_gauss(-N)", Storm::GetIndex::front_gauss, -N);
        MonkeyTimer::distribution_timer("middle_gauss(-N)", Storm::GetIndex::middle_gauss, -N);
        MonkeyTimer::distribution_timer("back_gauss(-N)", Storm::GetIndex::back_gauss, -N);
        MonkeyTimer::distribution_timer("quantum_gauss(-N)", Storm::GetIndex::quantum_gauss, -N);
        MonkeyTimer::distribution_timer("front_poisson(-N)", Storm::GetIndex::front_poisson, -N);
        MonkeyTimer::distribution_timer("middle_poisson(-N)", Storm::GetIndex::middle_poisson, -N);
        MonkeyTimer::distribution_timer("back_poisson(-N)", Storm::GetIndex::back_poisson, -N);
        MonkeyTimer::distribution_timer("quantum_poisson(-N)", Storm::GetIndex::quantum_poisson, -N);
        MonkeyTimer::distribution_timer("quantum_monty(-N)", Storm::GetIndex::quantum_monty, -N);
    }

    auto min_max_tests() -> void {
        IO::print("Min/Max Tests \n-------------\n");
        IO::print("Maximum Unsigned Integer: ", Storm::Meters::max_uint(), '\n');
        IO::print("Minimum Integer: ", Storm::Meters::min_int(), '\n');
        IO::print("Maximum Integer:  ", Storm::Meters::max_int(), '\n');
        IO::print("Minimum Float: ", Storm::Meters::min_float(), '\n');
        IO::print("Maximum Float:  ", Storm::Meters::max_float(), '\n');
        IO::print("Minimum Below: ", Storm::Meters::min_below(), '\n');
        IO::print("Minimum Above:  ", Storm::Meters::min_above(), '\n');
        IO::print("\n");
    }

    auto clamp_tests() -> void {
        IO::print("Clamp Tests \n-----------\n");
        MonkeyTimer::function_timer("std::clamp(3, 2, 1)", std::clamp<Storm::Integer>, 3, 2, 1);
        IO::print("is ", std::clamp<Storm::Integer>(3, 2, 1), " should be 2: ");
        IO::print(std::clamp<Storm::Float>(3, 2, 1) == 2 ? "Pass" : "Fail", "\n");
        MonkeyTimer::function_timer("clamp(3, 2, 1)", Storm::GearBox::clamp<Storm::Integer>, 3, 2, 1);
        IO::print("is ", Storm::GearBox::clamp<Storm::Integer>(3, 2, 1), " should be 2: ");
        IO::print(Storm::GearBox::clamp<Storm::Float>(3, 2, 1) == 2 ? "Pass" : "Fail", "\n\n");

        MonkeyTimer::function_timer("std::clamp(1.3, 1.2, 1.1)", std::clamp<Storm::Float>, 1.3, 1.2, 1.1);
        IO::print("is ", std::clamp<Storm::Float>(1.3, 1.2, 1.1), " should be 1.2: ");
        IO::print(std::clamp<Storm::Float>(1.3, 1.2, 1.1) == 1.2 ? "Pass" : "Fail", "\n");
        MonkeyTimer::function_timer("clamp(1.3, 1.2, 1.1)", Storm::GearBox::clamp<Storm::Float>, 1.3, 1.2, 1.1);
        IO::print("is ", Storm::GearBox::clamp<Storm::Float>(1.3, 1.2, 1.1), " should be 1.2: ");
        IO::print(Storm::GearBox::clamp<Storm::Float>(1.3, 1.2, 1.1) == 1.2 ? "Pass" : "Fail", "\n\n");
    }

    auto run_tests() -> void {
        MonkeyTimer::ScopeTimer t("Total Time");
        IO::print("\nMonkeyTimer");
        IO::print("\nStorm Version: ", Storm::version);
        IO::print("\n===========\n\n");
        StormTests::min_max_tests();
        StormTests::bool_tests();
        StormTests::integer_tests();
        StormTests::random_index_tests();
        StormTests::float_tests();
        StormTests::clamp_tests();
        IO::print("============================\n");
    }

}
