# Storm
## Random Number Engine & Toolkit
### C++ Header Only Library
### Author: Robert Sharp
#### © 2024 Robert Sharp, all rights reserved.

---

## Dev Notes

Storm is intended to provide thread compatible, high-performance tools to make custom random number generators for 
Python3. Storm does not directly interact with the Python runtime and therefore requires additional tools like Cython 
to bridge the gap. Storm is optimized for multithread execution on 64bit platforms.

### Warning: Storm is not intended for secure hashing or encryption of any kind!
That said, Storm is far stronger than the vanilla Mersenne Twister implementation.

For Python users, the way to get Storm is to install Fortuna. Fortuna is a Python c-extension (Cython wrapper for Storm). 
Fortuna features a collection of high-level abstractions for creating custom random generators that can model 
any distribution across any dataset. Fortuna is fully generic and can be used for generating all Python value 
types including custom objects. Fortuna supports multidimensional distributions, nested abstractions, dynamic 
flattening, lazy evaluation and dependency injection. 

### Fortuna Quick Install
- `$ pip install Fortuna`

### Storm Project File List
- `Storm.hpp` complete core functionality
  - `GetBool` boolean variant namespace
  - `GetInt` long long variant namespace
  - `GetFloat` double variant namespace
  - `GetIndex` ZeroCool distribution methods
- `MonkeyTimer.hpp` distribution/timer framework
- `StormTests.hpp` test implementation
- `Utilities.hpp` general utility tools, e.g. `print(...)`
- `main.cpp` executes a series of distribution and performance tests
- `README.md` this file
- `LICENSE` free and open for non-commercial use

### Distribution Range Notation:
- Inclusive Range: `[A, Z]` is the range A to Z including both A and Z.
- Exclusive Range: `(A, Z)` is the range A to Z excluding both A and Z.
- Tail Exclusion: `[A, Z)` is the range A to Z excluding Z.
- Head Exclusion: `(A, Z]` is the range A to Z excluding A.

---

## Storm Library

### Storm::Engine
- `Strom::Engine::Hurricane`, intended for internal use only.<br>
Storm is powered by The Hurricane Engine, a customizable shuffle-drop configuration of the Mersenne Twister Algorithm. 
This shuffle-drop strategy makes Hurricane slightly less breakable than vanilla MTA. By default, Hurricane is configured 
to produce beautiful, hardware-seeded, sudo-random entropy at high-speed across multiple threads in parallel. Hurricane 
supports hardware entropy when available on the host platform, otherwise it will fall back to the system default.
- `Storm::Engine::seed(seed_value) -> void`<br>
Hardware seeding is on by default. This function seeds the random engine. When passed a seed_value other than zero, 
this function will employ a repeatable software seeding strategy, when passed a seed_value of 0, it will employ an 
unrepeatable hardware seeding strategy, if available.

---

### Storm::GearBox
- `Storm::GearBox::clamp(Number a, Number b, Number c) -> Number`<br>
Clamps target `a` between limits `b` and `c`. Equivalent to median of 3 values.
- `Storm::GearBox::approximation_clamp(Callable approximation_function, Integer target, Integer upper_bound) -> Integer`<br>
Clamps target to range(0, upper_bound) by calling approximation_function(upper_limit) if needed.
- `Storm::GearBox::analytic_continuation(Callable func, Integer input, Integer offset) -> Integer`<br>
Continues positive-only function to the negative number line with a variable offset.

---

## Random Generators

### Storm::GetBool
- `Storm::GetBool::percent_true(double percent) -> bool`<br>
The input parameter represents the <u>percentage</u> of `true` in the distribution of the output. Input is clamped in 
range `[0.0, 100.0]`. Input that falls outside this range will saturate the boundaries without error.

```cpp
#include <iostream>
#include "Storm.hpp"

int main() {
    // 75% chance to print "Meow", 25% chance to print "Woof"
    if (Storm::GetBool::percent_true(75.0)) std::cout << "Meow" << "\n";
    else std::cout << "Woof" << "\n";
}
```

### Storm::GetInt
- `Storm::GetInt::random_below(long long number) -> long long`<br>
Classic algorithm for generating a random number below a given value.<br>
Flat uniform distribution of the range `[0, number)`.

```cpp
#include <iostream>
#include "Storm.hpp"

int main() {
    // prints a random number in range [0, 10)
    std::cout << Storm::GetInt::random_below(10) << "\n";
}
```

- `Storm::GetInt::uniform_int_variate(long long a, long long b) -> long long`<br>
Flat uniform integer distribution in the range `[low, high]` where low = min(a, b) and high = max(b, a).

- `Storm::GetInt::random_range(long long start, long long stop, long long step) -> long long`<br>
Where `low = min(start, stop)`, `high = max(start, stop)`<br>
Flat uniform distribution of the range `[low, high) by step`.<br>
Passing a negative `step` will invert the phase of the output distribution to `(low, high] by step`.<br>
`random_range(1, 10, 2) -> [1, 10) by 2`, odd numbers 1-9. Same as `random_range(10, 1, 2)`.<br>
`random_range(1, 10, -2) -> (1, 10] by -2`, even numbers 2-10. Same as `random_range(10, 1, -2)`.

- `Storm::GetInt::plus_or_minus(long long number) -> long long`<br>
Flat uniform distribution in range `[-number, number]`, zero mean.

- `Storm::GetInt::plus_or_minus_linear(long long number) -> long long`<br>
Linear distribution in range `[-number, number]`, zero mean.

- `Storm::GetInt::plus_or_minus_gauss(long long number) -> long long`<br>
Gaussian distribution in range `[-number, number]`, zero mean.

- `Storm::GetInt::binomial_variate(long long number_of_trials, double probability) -> long long`<br>
Binomial distribution based on number of trials and probability.

- `Storm::GetInt::negative_binomial_variate(long long number_of_trials, double probability) -> long long`<br>
Negative binomial distribution based on number of trials and probability.

- `Storm::GetInt::geometric_variate(double probability) -> long long`<br>
Geometric distribution based on probability.

- `Storm::GetInt::poisson_variate(double mean) -> long long`<br>
Poisson distribution based on mean.


### Dice
- `Storm::GetInt::d(long long sides) -> long long`<br>
Represents rolling a uniform multi-sided dice. `d(6)` is a regular six-sided die.<br>
Flat uniform distribution of the range `[1, sides]`.<br>
By definition, `d(0)` will always return zero.<br>
Negative input produces the inverse output range. `d(-6) -> [-6, -1]`

- `Storm::GetInt::dice(long long rolls, long long sides) -> long long`<br>
Computes the sum of multiple rolls of a multi-sided dice.<br>
`dice(3, 6)` is commonly written as 3d6 this is the same as a three six-sided dice rolled separately and added together.

- `Storm::GetInt::ability_dice(long long number) -> long long`<br>
Input n is clamped in range `[3, 9]`<br>
Computes the sum of the top three of n `d(6)` rolls, often the input will be 4.<br>
Geometric distribution based on the number of six-sided dice rolled.<br>
Output will always be in range `[3, 18]`, the mean will increase with higher input values.

### ZeroCool Specification
ZeroCool Specification: All ZeroCool functors are pure functions with the following signature: `F(N) -> N` they take a 
long and return a random long in range `[0, N)` and `[N, 0)` for negative N. Each ZeroCool functor models 
a unique distribution.

This specification is defined for the purpose of documenting the proper mapping of input to random output range such 
that - ZeroCool functors always produce valid python list indices that range from the front of a list for positive n, 
or the back for negative n, given |n| is less than or equal to the size of a non-empty list.

ZeroCool(0) is undefined: returning a sentinel, zero or negative one or raising an error or throwing an exception are 
all valid options as zero is a singularity in terms of distribution size. The following ZeroCool functors will return 
minus one for input zero, but this is not guaranteed or required by the spec. In most cases it simply never matters 
what a ZeroCool functor returns with zero for input, it could be indicative of an error but not necessarily.

#### Flat Uniform
- `Storm::GetInt::random_index(long long) -> long long`: Flat Line<br>

#### Linear Positional
- `Storm::GetInt::front_linear(long long) -> long long`: Left Triangle<br>
- `Storm::GetInt::middle_linear(long long) -> long long` Pyramid<br>
- `Storm::GetInt::back_linear(long long) -> long long`: Right Triangle<br>
- `Storm::GetInt::quantum_linear(long long) -> long long`: Sawtooth, 3 way Monty<br>

#### Gaussian Positional
- `Storm::GetInt::front_gauss(long long) -> long long`: Left Gamma<br>
- `Storm::GetInt::middle_gauss(long long) -> long long`: Normal Gauss<br>
- `Storm::GetInt::back_gauss(long long) -> long long`: Right Gamma<br>
- `Storm::GetInt::quantum_gauss(long long) -> long long`: Gamma Wave, 3 way Monty<br>

#### Poisson Positional
- `Storm::GetInt::front_poisson(long long) -> long long`: Left Hill<br>
- `Storm::GetInt::middle_poisson(long long) -> long long`: Twin Peaks<br>
- `Storm::GetInt::back_poisson(long long) -> long long`: Right Hill<br>
- `Storm::GetInt::quantum_poisson(long long) -> long long`: Poisson Wave, 3 way Monty<br>

#### Quantum Monty
- `Storm::GetInt::quantum_monty(long long) -> long long`: 3 by 3 way Monty<br>

### ZeroCool Python Examples

#### Produce a front linear distribution of the last five values of a list
```
from Fortuna import front_linear


some_list = [1, 2, 3, 4, 5, 6, 7, 8, 9]
range_to = -5
print(some_list[front_linear(range_to)])
""" Expected Probability Graph
 *
 *  *
 *  *  *
 *  *  *  *
 *  *  *  *  *
 5  6  7  8  9   <-- last five values in some_list.
-5 -4 -3 -2 -1   <-- indices from front_linear(-5)
"""
```

#### Produce a back linear distribution of the first five values of a list
```
from Fortuna import back_linear


some_list = [1, 2, 3, 4, 5, 6, 7, 8, 9]
range_to = 5
print(some_list[back_linear(range_to)])
""" Expected Probability Graph
            *
         *  *
      *  *  *
   *  *  *  *
*  *  *  *  *
1  2  3  4  5   <-- first five values in some_list
0  1  2  3  4   <-- indices from back_linear(5)
"""
```

#### Produce a middle linear distribution of the whole list.
```
from Fortuna import middle_linear


some_list = [1, 2, 3, 4, 5, 6, 7, 8, 9]
range_to = len(some_list)
print(some_list[middle_linear(range_to)])
""" Expected Probability Graph

             *
          *  *  *
       *  *  *  *  *
    *  *  *  *  *  *  *
 *  *  *  *  *  *  *  *  *
 1  2  3  4  5  6  7  8  9   <-- values in some_list
 0  1  2  3  4  5  6  7  8   <-- indices from middle_linear(9)
-9 -8 -7 -6 -5 -4 -3 -2 -1   <-- indices from middle_linear(-9)
"""
```


### Storm::GetFloat
`Storm::GetFloat::canonical_variate() -> double`<br>
Flat uniform distribution of the range `(0.0, 1.0)`.

`Storm::GetFloat::uniform_real_variate(double a, double b) -> double`<br>
Flat uniform distribution of the range `[a, b)`.

`Storm::GetFloat::exponential_variate(double lambda_rate) -> double`<br>
Exponential distribution based on lambda rate.

`Storm::GetFloat::gamma_variate(double shape, double scale) -> double`<br>
Gamma distribution based on shape and scale.

`Storm::GetFloat::weibull_variate(double shape, double scale) -> double`<br>
Weibull distribution based on shape and scale.

`Storm::GetFloat::normal_variate(double mean, double std_dev) -> double`<br>
Normal or Gaussian distribution based on mean and standard deviation.

`Storm::GetFloat::log_normal_variate(double log_mean, double log_deviation) -> double`<br>
Log Normal distribution based on log mean and log standard deviation.

`Storm::GetFloat::extreme_value_variate(double location, double scale) -> double`<br>
Extreme value distribution based on location and scale.

`Storm::GetFloat::chi_squared_variate(double degrees_of_freedom) -> double`<br>
Chi squared distribution based on degrees of freedom.

`Storm::GetFloat::cauchy_variate(double location, double scale) -> double`<br>
Cauchy distribution based on location and scale.

`Storm::GetFloat::fisher_f_variate(double degrees_of_freedom_1, double degrees_of_freedom_2) -> double`<br>
Fisher F distribution based on degrees of freedom across two dimensions.

`Storm::GetFloat::student_t_variate(double degrees_of_freedom) -> double`<br>
Student T distribution based on degrees of freedom.

`Storm::GetFloat::beta_variate(double alpha, double beta) -> double`<br>
Beta distribution based on two inputs alpha and beta.

`Storm::GetFloat::pareto_variate(double alpha) -> double`<br>
Pareto distribution based on input alpha.

`Storm::GetFloat::vonmises_variate(double mu, double kappa) -> double`<br>
Von Mises distribution based on two inputs: mu and kappa.

`Storm::GetFloat::triangular_variate(double low, double high, double mode) -> double`<br>
Triangular distribution based on low, high and mode.


## Performance and Distribution Tests via MonkeyTimer
Testbed Info
- Hardware: M1 Ultra, 20 Core CPU, 48 Core GPU, 128GB RAM, 4TB SSD, 32 Core Neural Engine
- Software: MacOS 13.3, MonkeyTimer, Storm

```
MonkeyTimer
Storm Version: 3.9.0
===========

Min/Max Tests 
-------------
Maximum Unsigned Integer: 18446744073709551615
Minimum Integer: -9223372036854775807
Maximum Integer:  9223372036854775807
Minimum Float: -1.79769e+308
Maximum Float:  1.79769e+308
Minimum Below: -4.94066e-324
Minimum Above:  4.94066e-324

Random Boolean 
--------------
percent_true(25.0): 6 nano
0: 75.051%
1: 24.949%

bernoulli_variate(0.25): 6 nano
0: 74.868%
1: 25.132%

Random Integer 
--------------
random_below(10): 20 nano
0: 10.123%
1: 9.962%
2: 10.008%
3: 10.088%
4: 10.085%
5: 9.905%
6: 9.87%
7: 9.998%
8: 9.921%
9: 10.04%

uniform_int_variate(0, 9): 15 nano
0: 10.048%
1: 10.09%
2: 9.961%
3: 10.039%
4: 9.908%
5: 10.073%
6: 9.981%
7: 9.924%
8: 10.12%
9: 9.856%

random_range(0, 20, 2): 25 nano
0: 9.909%
2: 9.905%
4: 10.177%
6: 10.001%
8: 9.849%
10: 10.143%
12: 9.961%
14: 10.197%
16: 9.829%
18: 10.029%

random_range(0, 20, -2): 24 nano
2: 9.886%
4: 10.217%
6: 10.003%
8: 10.117%
10: 10.084%
12: 9.879%
14: 10.015%
16: 9.824%
18: 9.892%
20: 10.083%

random_range(0, -20, 2): 24 nano
-20: 10.232%
-18: 9.921%
-16: 10.076%
-14: 9.944%
-12: 9.955%
-10: 9.962%
-8: 10.068%
-6: 9.986%
-4: 9.968%
-2: 9.888%

random_range(0, -20, -2): 25 nano
-18: 10.096%
-16: 10.143%
-14: 9.903%
-12: 9.996%
-10: 9.977%
-8: 10.068%
-6: 9.797%
-4: 9.95%
-2: 10.036%
0: 10.034%

plus_or_minus(10): 14 nano
-10: 4.767%
-9: 4.814%
-8: 4.762%
-7: 4.637%
-6: 4.831%
-5: 4.74%
-4: 4.749%
-3: 4.701%
-2: 4.768%
-1: 4.827%
0: 4.675%
1: 4.803%
2: 4.8%
3: 4.717%
4: 4.75%
5: 4.911%
6: 4.798%
7: 4.811%
8: 4.663%
9: 4.742%
10: 4.734%

plus_or_minus_linear(10): 26 nano
-10: 0.744%
-9: 1.7%
-8: 2.447%
-7: 3.328%
-6: 4.169%
-5: 4.983%
-4: 5.782%
-3: 6.585%
-2: 7.407%
-1: 8.224%
0: 9.127%
1: 8.272%
2: 7.387%
3: 6.654%
4: 5.784%
5: 4.844%
6: 4.182%
7: 3.209%
8: 2.578%
9: 1.694%
10: 0.9%

plus_or_minus_gauss(10): 29 nano
-10: 0.085%
-9: 0.218%
-8: 0.525%
-7: 1.147%
-6: 2.127%
-5: 3.7%
-4: 5.528%
-3: 7.947%
-2: 10.14%
-1: 11.854%
0: 12.73%
1: 11.921%
2: 10.593%
3: 7.844%
4: 5.777%
5: 3.616%
6: 2.17%
7: 1.148%
8: 0.574%
9: 0.254%
10: 0.102%

binomial_variate(10, 0.5): 84 nano
0: 0.112%
1: 0.961%
2: 4.49%
3: 11.644%
4: 20.624%
5: 24.613%
6: 20.654%
7: 11.492%
8: 4.338%
9: 0.968%
10: 0.104%

negative_binomial_variate(5, 0.75): 42 nano
0: 23.597%
1: 29.526%
2: 22.342%
3: 13.035%
4: 6.513%
5: 2.99%
6: 1.232%
7: 0.488%
8: 0.18%
9: 0.063%
10: 0.023%
11: 0.006%
12: 0.002%
13: 0.002%
14: 0.001%

geometric_variate(0.75): 14 nano
0: 74.849%
1: 18.824%
2: 4.734%
3: 1.211%
4: 0.288%
5: 0.076%
6: 0.015%
7: 0.003%

poisson_variate(4.0): 46 nano
0: 1.862%
1: 7.301%
2: 14.761%
3: 19.484%
4: 19.536%
5: 15.559%
6: 10.434%
7: 5.937%
8: 2.993%
9: 1.315%
10: 0.554%
11: 0.18%
12: 0.06%
13: 0.018%
14: 0.006%

Random Dice 
-----------
d(10): 14 nano
1: 10.086%
2: 9.999%
3: 9.992%
4: 10.087%
5: 10.13%
6: 10.018%
7: 9.926%
8: 10.043%
9: 9.804%
10: 9.915%

dice(3, 6): 35 nano
3: 0.436%
4: 1.454%
5: 2.759%
6: 4.538%
7: 7.067%
8: 9.448%
9: 11.516%
10: 12.527%
11: 12.404%
12: 11.895%
13: 9.772%
14: 6.855%
15: 4.724%
16: 2.724%
17: 1.4%
18: 0.481%

dice(0, 6): 1 nano
0: 100%

ability_dice(4): 94 nano
3: 0.061%
4: 0.342%
5: 0.738%
6: 1.595%
7: 2.869%
8: 4.845%
9: 7.032%
10: 9.522%
11: 11.356%
12: 12.922%
13: 13.214%
14: 12.39%
15: 10.223%
16: 7.189%
17: 4.202%
18: 1.5%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 20 nano
0: 9.956%
1: 10.086%
2: 9.961%
3: 10.022%
4: 9.988%
5: 10.002%
6: 10.105%
7: 9.894%
8: 9.892%
9: 10.094%

front_linear(N): 6 nano
0: 19.008%
1: 16.972%
2: 14.906%
3: 12.988%
4: 11.257%
5: 8.944%
6: 6.939%
7: 4.987%
8: 2.978%
9: 1.021%

middle_linear(N): 7 nano
0: 2.047%
1: 6.138%
2: 10.053%
3: 14.051%
4: 18.043%
5: 17.712%
6: 14.076%
7: 9.98%
8: 5.903%
9: 1.997%

back_linear(N): 6 nano
0: 1.021%
1: 3.059%
2: 5.096%
3: 7.013%
4: 8.858%
5: 10.906%
6: 13.055%
7: 15.049%
8: 17.047%
9: 18.896%

quantum_linear(N): 23 nano
0: 7.456%
1: 8.688%
2: 9.936%
3: 11.192%
4: 12.682%
5: 12.843%
6: 11.236%
7: 9.995%
8: 8.673%
9: 7.299%

front_gauss(N): 14 nano
0: 63.407%
1: 23.119%
2: 8.497%
3: 3.189%
4: 1.142%
5: 0.4%
6: 0.151%
7: 0.07%
8: 0.018%
9: 0.007%

middle_gauss(N): 26 nano
0: 0.004%
1: 0.131%
2: 2.115%
3: 13.728%
4: 34.05%
5: 34.142%
6: 13.565%
7: 2.122%
8: 0.141%
9: 0.002%

back_gauss(N): 14 nano
0: 0.008%
1: 0.022%
2: 0.057%
3: 0.15%
4: 0.424%
5: 1.127%
6: 3.146%
7: 8.571%
8: 23.454%
9: 63.041%

quantum_gauss(N): 36 nano
0: 21.205%
1: 7.613%
2: 3.717%
3: 5.785%
4: 11.905%
5: 11.891%
6: 5.646%
7: 3.558%
8: 7.699%
9: 20.981%

front_poisson(N): 41 nano
0: 8.417%
1: 20.439%
2: 25.803%
3: 21.415%
4: 13.194%
5: 6.539%
6: 2.806%
7: 1.01%
8: 0.301%
9: 0.076%

middle_poisson(N): 55 nano
0: 4.149%
1: 10.448%
2: 13.251%
3: 12.19%
4: 9.955%
5: 10.012%
6: 12.051%
7: 13.249%
8: 10.437%
9: 4.258%

back_poisson(N): 41 nano
0: 0.088%
1: 0.291%
2: 0.973%
3: 2.765%
4: 6.705%
5: 13.419%
6: 21.333%
7: 25.68%
8: 20.582%
9: 8.164%

quantum_poisson(N): 67 nano
0: 4.187%
1: 10.566%
2: 13.257%
3: 11.991%
4: 10.08%
5: 10.05%
6: 12.213%
7: 13.193%
8: 10.324%
9: 4.139%

quantum_monty(N): 55 nano
0: 10.725%
1: 9.068%
2: 9.02%
3: 9.652%
4: 11.707%
5: 11.502%
6: 9.85%
7: 8.83%
8: 8.87%
9: 10.776%

F(-N) where N = 10
random_index(-N): 20 nano
-10: 9.997%
-9: 9.977%
-8: 10.145%
-7: 10.074%
-6: 9.828%
-5: 9.888%
-4: 10.041%
-3: 10.002%
-2: 10.151%
-1: 9.897%

front_linear(-N): 7 nano
-10: 19.029%
-9: 17.007%
-8: 15.038%
-7: 12.762%
-6: 10.992%
-5: 9.024%
-4: 7.086%
-3: 4.995%
-2: 3.065%
-1: 1.002%

middle_linear(-N): 7 nano
-10: 1.932%
-9: 5.975%
-8: 9.975%
-7: 14.139%
-6: 17.904%
-5: 17.831%
-4: 14.104%
-3: 10.079%
-2: 6.037%
-1: 2.024%

back_linear(-N): 7 nano
-10: 0.959%
-9: 3.035%
-8: 5.031%
-7: 6.845%
-6: 8.789%
-5: 10.97%
-4: 13.066%
-3: 15.276%
-2: 17.049%
-1: 18.98%

quantum_linear(-N): 23 nano
-10: 7.308%
-9: 8.867%
-8: 9.856%
-7: 11.374%
-6: 12.725%
-5: 12.541%
-4: 11.396%
-3: 9.95%
-2: 8.576%
-1: 7.407%

front_gauss(-N): 14 nano
-10: 63.127%
-9: 23.305%
-8: 8.667%
-7: 3.109%
-6: 1.143%
-5: 0.417%
-4: 0.147%
-3: 0.051%
-2: 0.022%
-1: 0.012%

middle_gauss(-N): 28 nano
-10: 0.003%
-9: 0.12%
-8: 2.085%
-7: 13.674%
-6: 34.133%
-5: 33.955%
-4: 13.696%
-3: 2.206%
-2: 0.123%
-1: 0.005%

back_gauss(-N): 15 nano
-10: 0.003%
-9: 0.027%
-8: 0.064%
-7: 0.17%
-6: 0.431%
-5: 1.179%
-4: 3.12%
-3: 8.703%
-2: 23.287%
-1: 63.016%

quantum_gauss(-N): 35 nano
-10: 20.867%
-9: 7.726%
-8: 3.645%
-7: 5.653%
-6: 12.049%
-5: 12.098%
-4: 5.457%
-3: 3.591%
-2: 7.793%
-1: 21.121%

front_poisson(-N): 41 nano
-10: 8.256%
-9: 20.538%
-8: 25.636%
-7: 21.465%
-6: 13.187%
-5: 6.702%
-4: 2.798%
-3: 1.001%
-2: 0.322%
-1: 0.095%

middle_poisson(-N): 54 nano
-10: 4.095%
-9: 10.367%
-8: 13.453%
-7: 12.032%
-6: 9.875%
-5: 10.122%
-4: 12.11%
-3: 13.35%
-2: 10.412%
-1: 4.184%

back_poisson(-N): 41 nano
-10: 0.081%
-9: 0.289%
-8: 1.005%
-7: 2.758%
-6: 6.744%
-5: 13.457%
-4: 21.226%
-3: 25.799%
-2: 20.409%
-1: 8.232%

quantum_poisson(-N): 64 nano
-10: 4.245%
-9: 10.257%
-8: 13.301%
-7: 12.057%
-6: 10.071%
-5: 9.862%
-4: 12.089%
-3: 13.289%
-2: 10.645%
-1: 4.184%

quantum_monty(-N): 57 nano
-10: 10.846%
-9: 8.956%
-8: 9.016%
-7: 9.682%
-6: 11.588%
-5: 11.44%
-4: 9.671%
-3: 8.979%
-2: 8.974%
-1: 10.848%

Random Float: timer only 
------------------------
canonical_variate(): 5 nano
uniform_real_variate(1.0, 10.0): 5 nano
exponential_variate(1.0): 12 nano
gamma_variate(1.0, 1.0): 13 nano
weibull_variate(1.0, 1.0): 23 nano
normal_variate(5, 2): 25 nano
log_normal_variate(1.6, 0.25): 30 nano
extreme_value_variate(0.0, 1.0): 24 nano
chi_squared_variate(1.0): 43 nano
cauchy_variate(0.0, 0.00005): 15 nano
fisher_f_variate(1.0, 1.0): 87 nano
student_t_variate(1.0): 67 nano
beta_variate(10.0, 1.0): 72 nano
pareto_variate(5.0): 5 nano
vonmises_variate(1.0, 1.0): 64 nano
triangular_variate(0.0, 10.0, 5.0): 6 nano

Clamp Tests (std::clamp is expected to fail) 
-----------
std::clamp(3, 2, 1): 0 nano
        is 1 should be 2: Fail
clamp(3, 2, 1): 0 nano
        is 2 should be 2: Pass

std::clamp(1.3, 1.2, 1.1): 0 nano
        is 1.1 should be 1.2: Fail
clamp(1.3, 1.2, 1.1): 0 nano
        is 1.2 should be 1.2: Pass

============================
Total Time: 0.395608 seconds
```
