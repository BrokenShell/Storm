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

<blockquote>
Note: The newest version of MonkeyTimer is far more precise than previous versions. The following tests are not directly comparable to previous versions of Storm.
</blockquote>

#### Testbed Info
- Hardware: M1 Ultra, 20 Core CPU, 48 Core GPU, 128GB RAM, 4TB SSD, 32 Core Neural Engine
- Software: macOS 14.3.1, MonkeyTimer, Storm

```
MonkeyTimer
Storm Version: 3.9.2
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
0: 74.919%
1: 25.081%

percent_true(25.0): 11.3688 nano
0: 74.871%
1: 25.129%

bernoulli_variate(0.25): 10.9046 nano
Random Integer 
--------------
0: 9.979%
1: 10.093%
2: 10.088%
3: 9.863%
4: 10.091%
5: 9.905%
6: 10.024%
7: 10.122%
8: 9.803%
9: 10.032%

random_below(10): 31.3542 nano
0: 10.079%
1: 9.867%
2: 10.081%
3: 9.846%
4: 10.065%
5: 10.227%
6: 9.995%
7: 9.975%
8: 9.88%
9: 9.985%

uniform_int_variate(0, 9): 21.1042 nano
0: 10.069%
2: 10.059%
4: 10.006%
6: 9.957%
8: 9.937%
10: 10.077%
12: 9.996%
14: 10.036%
16: 9.908%
18: 9.955%

random_range(0, 20, 2): 31.1342 nano
2: 9.919%
4: 10.065%
6: 9.985%
8: 10.054%
10: 9.98%
12: 9.91%
14: 9.992%
16: 10.035%
18: 10.094%
20: 9.966%

random_range(0, 20, -2): 28.1592 nano
-20: 9.868%
-18: 10.295%
-16: 10.079%
-14: 10.025%
-12: 9.957%
-10: 9.984%
-8: 10.093%
-6: 9.881%
-4: 9.911%
-2: 9.907%

random_range(0, -20, 2): 27.3554 nano
-18: 9.81%
-16: 10.219%
-14: 9.953%
-12: 10.096%
-10: 9.936%
-8: 10.212%
-6: 9.79%
-4: 10.018%
-2: 10.03%
0: 9.936%

random_range(0, -20, -2): 26.1008 nano
-10: 4.74%
-9: 4.8%
-8: 4.751%
-7: 4.722%
-6: 4.787%
-5: 4.755%
-4: 4.894%
-3: 4.845%
-2: 4.689%
-1: 4.823%
0: 4.667%
1: 4.702%
2: 4.801%
3: 4.71%
4: 4.83%
5: 4.704%
6: 4.739%
7: 4.762%
8: 4.705%
9: 4.821%
10: 4.753%

plus_or_minus(10): 14.1454 nano
-10: 0.8%
-9: 1.645%
-8: 2.476%
-7: 3.23%
-6: 4.132%
-5: 4.847%
-4: 5.931%
-3: 6.491%
-2: 7.414%
-1: 8.465%
0: 9.194%
1: 8.331%
2: 7.403%
3: 6.578%
4: 5.772%
5: 5.109%
6: 4.051%
7: 3.222%
8: 2.418%
9: 1.607%
10: 0.884%

plus_or_minus_linear(10): 25.5904 nano
-10: 0.076%
-9: 0.246%
-8: 0.521%
-7: 1.141%
-6: 2.118%
-5: 3.646%
-4: 5.754%
-3: 8.093%
-2: 10.213%
-1: 11.917%
0: 12.526%
1: 11.896%
2: 10.262%
3: 7.99%
4: 5.716%
5: 3.767%
6: 2.17%
7: 1.102%
8: 0.537%
9: 0.208%
10: 0.101%

plus_or_minus_gauss(10): 29.7 nano
0: 0.111%
1: 0.925%
2: 4.404%
3: 11.833%
4: 20.502%
5: 24.607%
6: 20.351%
7: 11.849%
8: 4.292%
9: 1.039%
10: 0.087%

binomial_variate(10, 0.5): 83.0533 nano
0: 23.726%
1: 29.592%
2: 22.151%
3: 12.938%
4: 6.481%
5: 3.048%
6: 1.28%
7: 0.5%
8: 0.175%
9: 0.069%
10: 0.023%
11: 0.013%
12: 0.001%
13: 0.002%
17: 0.001%

negative_binomial_variate(5, 0.75): 42.0925 nano
0: 75.044%
1: 18.682%
2: 4.711%
3: 1.17%
4: 0.303%
5: 0.069%
6: 0.015%
7: 0.005%
8: 0.001%

geometric_variate(0.75): 14.0025 nano
0: 1.861%
1: 7.292%
2: 14.514%
3: 19.665%
4: 19.515%
5: 15.747%
6: 10.377%
7: 6.032%
8: 2.897%
9: 1.337%
10: 0.476%
11: 0.178%
12: 0.073%
13: 0.027%
14: 0.005%
15: 0.003%
16: 0.001%

poisson_variate(4.0): 43.2546 nano
Random Dice 
-----------
1: 9.959%
2: 9.996%
3: 10.042%
4: 10.09%
5: 9.765%
6: 10.016%
7: 10.065%
8: 10.061%
9: 10.05%
10: 9.956%

d(10): 13.9354 nano
3: 0.45%
4: 1.399%
5: 2.782%
6: 4.617%
7: 6.939%
8: 9.596%
9: 11.497%
10: 12.582%
11: 12.513%
12: 11.569%
13: 9.891%
14: 6.861%
15: 4.682%
16: 2.709%
17: 1.421%
18: 0.492%

dice(3, 6): 33.6921 nano
0: 100%

dice(0, 6): 1.60417 nano
3: 0.084%
4: 0.325%
5: 0.759%
6: 1.643%
7: 3.01%
8: 4.88%
9: 6.962%
10: 9.387%
11: 11.28%
12: 13.026%
13: 13.237%
14: 12.234%
15: 10.29%
16: 7.136%
17: 4.15%
18: 1.597%

ability_dice(4): 87.5812 nano
Random Index: ZeroCool 
----------------------
F(N) where N = 10
0: 10.066%
1: 10.02%
2: 9.938%
3: 10.088%
4: 10.082%
5: 9.864%
6: 9.981%
7: 9.988%
8: 10.04%
9: 9.933%

random_index(N): 19.4104 nano
0: 18.937%
1: 17.08%
2: 14.968%
3: 13.057%
4: 10.921%
5: 8.975%
6: 6.843%
7: 5.09%
8: 3.105%
9: 1.024%

front_linear(N): 6.655 nano
0: 1.977%
1: 6.106%
2: 9.747%
3: 14.023%
4: 17.978%
5: 18.105%
6: 14.083%
7: 9.976%
8: 5.955%
9: 2.05%

middle_linear(N): 15.4479 nano
0: 0.991%
1: 2.987%
2: 5.01%
3: 7.153%
4: 9.022%
5: 11.058%
6: 12.915%
7: 14.992%
8: 16.924%
9: 18.948%

back_linear(N): 6.76875 nano
0: 7.246%
1: 8.683%
2: 9.892%
3: 11.467%
4: 12.843%
5: 12.608%
6: 11.305%
7: 9.958%
8: 8.589%
9: 7.409%

quantum_linear(N): 25.8792 nano
0: 63.098%
1: 23.396%
2: 8.497%
3: 3.139%
4: 1.2%
5: 0.442%
6: 0.148%
7: 0.051%
8: 0.019%
9: 0.01%

front_gauss(N): 14.5067 nano
0: 0.002%
1: 0.122%
2: 2.217%
3: 13.671%
4: 33.95%
5: 33.977%
6: 13.726%
7: 2.192%
8: 0.137%
9: 0.006%

middle_gauss(N): 26.5792 nano
0: 0.011%
1: 0.021%
2: 0.057%
3: 0.168%
4: 0.408%
5: 1.158%
6: 3.15%
7: 8.474%
8: 23.233%
9: 63.32%

back_gauss(N): 14.4292 nano
0: 20.847%
1: 7.883%
2: 3.65%
3: 5.619%
4: 11.903%
5: 11.788%
6: 5.488%
7: 3.636%
8: 7.798%
9: 21.388%

quantum_gauss(N): 34.4054 nano
0: 8.231%
1: 20.485%
2: 25.526%
3: 21.411%
4: 13.446%
5: 6.66%
6: 2.858%
7: 0.992%
8: 0.314%
9: 0.077%

front_poisson(N): 40.2175 nano
0: 4.166%
1: 10.24%
2: 13.379%
3: 11.887%
4: 10.182%
5: 10.009%
6: 12.007%
7: 13.451%
8: 10.439%
9: 4.24%

middle_poisson(N): 53.4213 nano
0: 0.074%
1: 0.289%
2: 0.981%
3: 2.726%
4: 6.67%
5: 13.452%
6: 21.421%
7: 25.657%
8: 20.677%
9: 8.053%

back_poisson(N): 39.3775 nano
0: 4.136%
1: 10.315%
2: 13.395%
3: 12.119%
4: 10.163%
5: 9.949%
6: 11.96%
7: 13.46%
8: 10.373%
9: 4.13%

quantum_poisson(N): 59.8271 nano
0: 10.968%
1: 8.977%
2: 8.88%
3: 9.536%
4: 11.419%
5: 11.574%
6: 9.621%
7: 9.033%
8: 9.133%
9: 10.859%

quantum_monty(N): 57.5292 nano
F(-N) where N = 10
-10: 9.999%
-9: 10.064%
-8: 10.028%
-7: 10.007%
-6: 10.087%
-5: 9.982%
-4: 10.148%
-3: 9.796%
-2: 9.856%
-1: 10.033%

random_index(-N): 20.3742 nano
-10: 18.844%
-9: 17.087%
-8: 14.971%
-7: 13.071%
-6: 10.825%
-5: 9.114%
-4: 7.043%
-3: 5.046%
-2: 2.96%
-1: 1.039%

front_linear(-N): 6.71041 nano
-10: 1.989%
-9: 5.96%
-8: 10.167%
-7: 14.034%
-6: 17.868%
-5: 17.894%
-4: 14.172%
-3: 9.99%
-2: 5.961%
-1: 1.965%

middle_linear(-N): 15.8783 nano
-10: 0.971%
-9: 2.941%
-8: 4.918%
-7: 6.982%
-6: 8.988%
-5: 10.954%
-4: 13.096%
-3: 15.349%
-2: 16.928%
-1: 18.873%

back_linear(-N): 7.185 nano
-10: 7.246%
-9: 8.652%
-8: 10.044%
-7: 11.387%
-6: 12.528%
-5: 12.87%
-4: 11.519%
-3: 9.864%
-2: 8.585%
-1: 7.305%

quantum_linear(-N): 25.6733 nano
-10: 63.148%
-9: 23.33%
-8: 8.595%
-7: 3.174%
-6: 1.08%
-5: 0.456%
-4: 0.146%
-3: 0.039%
-2: 0.02%
-1: 0.012%

front_gauss(-N): 14.3946 nano
-10: 0.005%
-9: 0.138%
-8: 2.202%
-7: 13.525%
-6: 34.263%
-5: 34.068%
-4: 13.572%
-3: 2.098%
-2: 0.128%
-1: 0.001%

middle_gauss(-N): 27.2862 nano
-10: 0.007%
-9: 0.021%
-8: 0.059%
-7: 0.15%
-6: 0.402%
-5: 1.104%
-4: 3.163%
-3: 8.548%
-2: 23.351%
-1: 63.195%

back_gauss(-N): 14.5183 nano
-10: 21%
-9: 7.872%
-8: 3.635%
-7: 5.65%
-6: 12.071%
-5: 11.837%
-4: 5.671%
-3: 3.563%
-2: 7.678%
-1: 21.023%

quantum_gauss(-N): 34.7825 nano
-10: 8.243%
-9: 20.579%
-8: 25.408%
-7: 21.534%
-6: 13.492%
-5: 6.595%
-4: 2.723%
-3: 1.027%
-2: 0.308%
-1: 0.091%

front_poisson(-N): 40.3767 nano
-10: 4.033%
-9: 10.439%
-8: 13.266%
-7: 12.118%
-6: 10.057%
-5: 9.987%
-4: 12.198%
-3: 13.409%
-2: 10.335%
-1: 4.158%

middle_poisson(-N): 53.0588 nano
-10: 0.094%
-9: 0.306%
-8: 1.012%
-7: 2.733%
-6: 6.672%
-5: 13.4%
-4: 21.352%
-3: 25.594%
-2: 20.663%
-1: 8.174%

back_poisson(-N): 40.0225 nano
-10: 4.163%
-9: 10.28%
-8: 13.272%
-7: 12.205%
-6: 10.192%
-5: 10.033%
-4: 11.998%
-3: 13.209%
-2: 10.53%
-1: 4.118%

quantum_poisson(-N): 59.9979 nano
-10: 10.826%
-9: 9.011%
-8: 8.906%
-7: 9.635%
-6: 11.686%
-5: 11.525%
-4: 9.778%
-3: 8.921%
-2: 8.956%
-1: 10.756%

quantum_monty(-N): 56.5588 nano
Random Float: timer only 
------------------------
canonical_variate(): 5.82125 nano
uniform_real_variate(1.0, 10.0): 5.865 nano
exponential_variate(1.0): 12.4504 nano
gamma_variate(1.0, 1.0): 13.2954 nano
weibull_variate(1.0, 1.0): 23.4508 nano
normal_variate(5, 2): 25.0154 nano
log_normal_variate(1.6, 0.25): 29.3242 nano
extreme_value_variate(0.0, 1.0): 24.6671 nano
chi_squared_variate(1.0): 41.4387 nano
cauchy_variate(0.0, 0.00005): 14.6192 nano
fisher_f_variate(1.0, 1.0): 82.7729 nano
student_t_variate(1.0): 66.2471 nano
beta_variate(10.0, 1.0): 69.505 nano
pareto_variate(5.0): 5.93333 nano
vonmises_variate(1.0, 1.0): 58.5042 nano
triangular_variate(0.0, 10.0, 5.0): 5.73 nano

Clamp Tests (std::clamp is expected to fail) 
-----------
std::clamp(3, 2, 1): 0.00042 nano
        is 1 should be 2: Fail
clamp(3, 2, 1): 0 nano
        is 2 should be 2: Pass

std::clamp(1.3, 1.2, 1.1): 0 nano
        is 1.1 should be 1.2: Fail
clamp(1.3, 1.2, 1.1): 0.00042 nano
        is 1.2 should be 1.2: Pass

============================
Total Time: 0.396136 seconds
```
