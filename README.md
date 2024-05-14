# Storm
## Random Number Engine & Toolkit
### C++ Header Only Library
### Author: Robert Sharp
#### © 2024 Robert Sharp, all rights reserved.

---

## Dev Notes

Storm is intended to provide thread compatible, high-performance tools to make custom random number generators. 
Storm is optimized for multithread execution on 64bit platforms.

### Warning: Storm is not intended for secure hashing or encryption of any kind!
That said, Storm is far stronger than the vanilla Mersenne Twister implementation.

For Python users, the way to get Storm is to install Fortuna. Fortuna is a Python c-extension that includes Storm. 
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
- `LICENSE` free and open for non-commercial use, other licenses available

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
- `Storm::GetInt::quantum_monty(long long) -> long long`: 9 way Monty<br>

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
- Software: macOS 14.4.1, MonkeyTimer, Storm

```
MonkeyTimer
Storm Version: 4.0.1
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
percent_true(25.0): 5.84416 nano
0: 75.073%
1: 24.927%

bernoulli_variate(0.25): 5.98875 nano
0: 75.157%
1: 24.843%

Random Integer 
--------------
random_below(10): 14.525 nano
0: 10.015%
1: 10.023%
2: 9.975%
3: 10.109%
4: 10.024%
5: 9.91%
6: 9.878%
7: 10.021%
8: 9.907%
9: 10.138%

uniform_int_variate(0, 9): 14.2496 nano
0: 10.083%
1: 10.075%
2: 9.83%
3: 9.965%
4: 10.146%
5: 9.982%
6: 9.971%
7: 9.895%
8: 9.973%
9: 10.08%

uniform_uint_variate(0, 9): 14.2871 nano
0: 10.168%
1: 9.76%
2: 10.051%
3: 10.153%
4: 9.897%
5: 9.968%
6: 10.038%
7: 9.814%
8: 10.083%
9: 10.068%

random_range(0, 20, 2): 23.5287 nano
0: 10.072%
2: 9.982%
4: 10.152%
6: 9.904%
8: 10.086%
10: 9.974%
12: 9.906%
14: 10.047%
16: 9.981%
18: 9.896%

random_range(0, 20, -2): 23.8629 nano
2: 9.94%
4: 10.088%
6: 9.92%
8: 9.898%
10: 9.938%
12: 9.962%
14: 10.082%
16: 10.096%
18: 10.098%
20: 9.978%

random_range(0, -20, 2): 24.0779 nano
-20: 10.033%
-18: 9.934%
-16: 9.959%
-14: 9.918%
-12: 9.893%
-10: 10.003%
-8: 9.899%
-6: 10.035%
-4: 10.072%
-2: 10.254%

random_range(0, -20, -2): 24.165 nano
-18: 9.98%
-16: 10.063%
-14: 10.081%
-12: 9.989%
-10: 10.105%
-8: 9.915%
-6: 9.964%
-4: 10%
-2: 10.022%
0: 9.881%

plus_or_minus(10): 14.0767 nano
-10: 4.764%
-9: 4.862%
-8: 4.782%
-7: 4.746%
-6: 4.67%
-5: 4.82%
-4: 4.76%
-3: 4.853%
-2: 4.796%
-1: 4.73%
0: 4.775%
1: 4.798%
2: 4.723%
3: 4.788%
4: 4.737%
5: 4.674%
6: 4.695%
7: 4.748%
8: 4.775%
9: 4.721%
10: 4.783%

plus_or_minus_linear(10): 24.9217 nano
-10: 0.836%
-9: 1.651%
-8: 2.479%
-7: 3.294%
-6: 4.1%
-5: 4.921%
-4: 5.744%
-3: 6.594%
-2: 7.474%
-1: 8.243%
0: 9.216%
1: 8.267%
2: 7.443%
3: 6.521%
4: 5.815%
5: 4.937%
6: 4.157%
7: 3.328%
8: 2.542%
9: 1.582%
10: 0.856%

plus_or_minus_gauss(10): 29.0125 nano
-10: 0.089%
-9: 0.204%
-8: 0.559%
-7: 1.09%
-6: 2.093%
-5: 3.682%
-4: 5.767%
-3: 8.074%
-2: 10.343%
-1: 12.098%
0: 12.368%
1: 11.823%
2: 10.168%
3: 8.034%
4: 5.696%
5: 3.748%
6: 2.177%
7: 1.098%
8: 0.568%
9: 0.228%
10: 0.093%

binomial_variate(10, 0.5): 77.2704 nano
0: 0.091%
1: 0.947%
2: 4.339%
3: 11.855%
4: 20.534%
5: 24.478%
6: 20.558%
7: 11.773%
8: 4.356%
9: 0.972%
10: 0.097%

negative_binomial_variate(5, 0.75): 44.0167 nano
0: 23.569%
1: 29.442%
2: 22.399%
3: 13.122%
4: 6.506%
5: 2.953%
6: 1.318%
7: 0.433%
8: 0.155%
9: 0.068%
10: 0.025%
11: 0.007%
12: 0.001%
13: 0.002%

geometric_variate(0.75): 13.4692 nano
0: 75.078%
1: 18.747%
2: 4.673%
3: 1.146%
4: 0.282%
5: 0.058%
6: 0.01%
7: 0.003%
8: 0.002%
9: 0.001%

poisson_variate(4.0): 42.8287 nano
0: 1.876%
1: 7.261%
2: 14.752%
3: 19.523%
4: 19.357%
5: 15.648%
6: 10.46%
7: 5.937%
8: 3.122%
9: 1.251%
10: 0.526%
11: 0.192%
12: 0.072%
13: 0.014%
14: 0.007%
15: 0.002%

Random Dice 
-----------
d(10): 13.5883 nano
1: 10.129%
2: 10.031%
3: 10.049%
4: 9.879%
5: 10.032%
6: 9.825%
7: 10.014%
8: 9.966%
9: 10.015%
10: 10.06%

dice(3, 6): 33.535 nano
3: 0.455%
4: 1.394%
5: 2.755%
6: 4.688%
7: 6.98%
8: 9.75%
9: 11.601%
10: 12.467%
11: 12.337%
12: 11.55%
13: 9.755%
14: 6.943%
15: 4.646%
16: 2.791%
17: 1.415%
18: 0.473%

dice(0, 6): 1.56875 nano
0: 100%

ability_dice(4): 89.4387 nano
3: 0.067%
4: 0.321%
5: 0.778%
6: 1.647%
7: 2.951%
8: 4.724%
9: 7.125%
10: 9.301%
11: 11.508%
12: 12.975%
13: 13.241%
14: 12.276%
15: 9.993%
16: 7.202%
17: 4.205%
18: 1.686%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 20.1329 nano
0: 10.111%
1: 9.893%
2: 9.889%
3: 10.08%
4: 9.713%
5: 9.901%
6: 10.15%
7: 10.07%
8: 10.012%
9: 10.181%

front_linear(N): 6.54 nano
0: 18.894%
1: 17.294%
2: 14.784%
3: 12.917%
4: 10.908%
5: 9.02%
6: 7.094%
7: 5.026%
8: 3.052%
9: 1.011%

middle_linear(N): 14.7988 nano
0: 1.976%
1: 5.973%
2: 10.057%
3: 14.004%
4: 18.056%
5: 18.074%
6: 13.998%
7: 9.944%
8: 5.918%
9: 2%

back_linear(N): 6.44791 nano
0: 1.011%
1: 2.98%
2: 4.992%
3: 7.079%
4: 9.191%
5: 10.994%
6: 13.107%
7: 14.884%
8: 16.854%
9: 18.908%

quantum_linear(N): 25.38 nano
0: 7.327%
1: 8.588%
2: 10.004%
3: 11.43%
4: 12.76%
5: 12.696%
6: 11.239%
7: 10.032%
8: 8.617%
9: 7.307%

front_gauss(N): 14.2479 nano
0: 63.39%
1: 23.245%
2: 8.377%
3: 3.14%
4: 1.175%
5: 0.425%
6: 0.168%
7: 0.048%
8: 0.022%
9: 0.01%

middle_gauss(N): 26.8062 nano
0: 0.004%
1: 0.115%
2: 2.087%
3: 13.588%
4: 33.835%
5: 34.394%
6: 13.664%
7: 2.172%
8: 0.134%
9: 0.007%

back_gauss(N): 14.2188 nano
0: 0.006%
1: 0.022%
2: 0.053%
3: 0.153%
4: 0.416%
5: 1.1%
6: 3.27%
7: 8.701%
8: 23.16%
9: 63.119%

quantum_gauss(N): 34.2762 nano
0: 20.951%
1: 7.817%
2: 3.534%
3: 5.638%
4: 11.892%
5: 11.769%
6: 5.62%
7: 3.649%
8: 7.889%
9: 21.241%

front_poisson(N): 38.7783 nano
0: 8.287%
1: 20.452%
2: 25.733%
3: 21.273%
4: 13.469%
5: 6.647%
6: 2.763%
7: 0.984%
8: 0.297%
9: 0.095%

middle_poisson(N): 52.5712 nano
0: 4.172%
1: 10.394%
2: 13.211%
3: 12.121%
4: 10.128%
5: 9.981%
6: 11.999%
7: 13.365%
8: 10.457%
9: 4.172%

back_poisson(N): 39.0854 nano
0: 0.067%
1: 0.344%
2: 1.021%
3: 2.814%
4: 6.67%
5: 13.349%
6: 21.243%
7: 25.578%
8: 20.771%
9: 8.143%

quantum_poisson(N): 59.9579 nano
0: 4.173%
1: 10.365%
2: 13.197%
3: 12.26%
4: 10.199%
5: 10.004%
6: 12.148%
7: 13.423%
8: 10.241%
9: 3.99%

quantum_monty(N): 56.7854 nano
0: 10.83%
1: 8.872%
2: 9.119%
3: 9.727%
4: 11.703%
5: 11.447%
6: 9.636%
7: 9.003%
8: 9.007%
9: 10.656%

F(-N) where N = 10
random_index(-N): 20.6646 nano
-10: 10.036%
-9: 9.93%
-8: 10.182%
-7: 9.922%
-6: 9.938%
-5: 9.985%
-4: 9.937%
-3: 10.072%
-2: 10.054%
-1: 9.944%

front_linear(-N): 7.18916 nano
-10: 18.867%
-9: 17.068%
-8: 15.071%
-7: 13.1%
-6: 11.014%
-5: 8.902%
-4: 6.974%
-3: 4.993%
-2: 3.017%
-1: 0.994%

middle_linear(-N): 16.9133 nano
-10: 1.97%
-9: 5.958%
-8: 10.132%
-7: 13.892%
-6: 18.13%
-5: 17.918%
-4: 13.972%
-3: 9.941%
-2: 6.02%
-1: 2.067%

back_linear(-N): 7.10208 nano
-10: 0.974%
-9: 2.944%
-8: 5.113%
-7: 7.052%
-6: 8.85%
-5: 10.902%
-4: 13.208%
-3: 14.819%
-2: 17.267%
-1: 18.871%

quantum_linear(-N): 26.3238 nano
-10: 7.401%
-9: 8.61%
-8: 10.021%
-7: 11.356%
-6: 12.547%
-5: 12.548%
-4: 11.369%
-3: 10.064%
-2: 8.645%
-1: 7.439%

front_gauss(-N): 14.5658 nano
-10: 63.146%
-9: 23.306%
-8: 8.592%
-7: 3.149%
-6: 1.133%
-5: 0.435%
-4: 0.158%
-3: 0.049%
-2: 0.02%
-1: 0.012%

middle_gauss(-N): 29.6108 nano
-10: 0.001%
-9: 0.15%
-8: 2.138%
-7: 13.639%
-6: 34.12%
-5: 34.146%
-4: 13.642%
-3: 2.025%
-2: 0.136%
-1: 0.003%

back_gauss(-N): 14.565 nano
-10: 0.005%
-9: 0.023%
-8: 0.057%
-7: 0.157%
-6: 0.407%
-5: 1.124%
-4: 3.12%
-3: 8.489%
-2: 23.305%
-1: 63.313%

quantum_gauss(-N): 34.9021 nano
-10: 21.267%
-9: 7.861%
-8: 3.59%
-7: 5.528%
-6: 11.895%
-5: 11.99%
-4: 5.574%
-3: 3.539%
-2: 7.642%
-1: 21.114%

front_poisson(-N): 38.9138 nano
-10: 8.349%
-9: 20.399%
-8: 25.479%
-7: 21.501%
-6: 13.268%
-5: 6.672%
-4: 2.889%
-3: 1.021%
-2: 0.327%
-1: 0.095%

middle_poisson(-N): 55.0933 nano
-10: 4.049%
-9: 10.33%
-8: 13.353%
-7: 12.024%
-6: 9.993%
-5: 10.085%
-4: 12.138%
-3: 13.412%
-2: 10.516%
-1: 4.1%

back_poisson(-N): 39.9562 nano
-10: 0.086%
-9: 0.28%
-8: 1%
-7: 2.777%
-6: 6.669%
-5: 13.452%
-4: 21.258%
-3: 25.634%
-2: 20.643%
-1: 8.201%

quantum_poisson(-N): 62.5421 nano
-10: 4.148%
-9: 10.325%
-8: 13.369%
-7: 12.016%
-6: 10.106%
-5: 10.141%
-4: 12.022%
-3: 13.325%
-2: 10.512%
-1: 4.036%

quantum_monty(-N): 57.1854 nano
-10: 10.947%
-9: 9.031%
-8: 8.882%
-7: 9.826%
-6: 11.6%
-5: 11.497%
-4: 9.588%
-3: 8.895%
-2: 8.965%
-1: 10.769%

Random Float: timer only 
------------------------
canonical_variate(): 6.00667 nano
uniform_real_variate(1.0, 10.0): 5.7375 nano
exponential_variate(1.0): 12.6346 nano
gamma_variate(1.0, 1.0): 13.1658 nano
weibull_variate(1.0, 1.0): 23.9975 nano
normal_variate(5, 2): 25.5379 nano
log_normal_variate(1.6, 0.25): 29.62 nano
extreme_value_variate(0.0, 1.0): 24.4046 nano
chi_squared_variate(1.0): 41.4025 nano
cauchy_variate(0.0, 0.00005): 14.3287 nano
fisher_f_variate(1.0, 1.0): 81.7675 nano
student_t_variate(1.0): 65.1937 nano
beta_variate(10.0, 1.0): 69.1833 nano
pareto_variate(5.0): 5.69084 nano
vonmises_variate(1.0, 1.0): 59.2287 nano
triangular_variate(0.0, 10.0, 5.0): 5.68708 nano

Clamp Tests (std::clamp is expected to fail) 
-----------
std::clamp(3, 2, 1): 0 nano
	is 1 should be 2: Fail
clamp(3, 2, 1): 0 nano
	is 2 should be 2: Pass

std::clamp(1.3, 1.2, 1.1): 0.00042 nano
	is 1.1 should be 1.2: Fail
clamp(1.3, 1.2, 1.1): 0 nano
	is 1.2 should be 1.2: Pass

============================
Total Time: 0.383729 seconds
```
