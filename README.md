# Storm
## Random Number Engine & Toolkit
### C++ Header Only Library
### Author: Robert Sharp
#### © 2023 Robert Sharp, all rights reserved.

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
- `Strom::Engine::Typhoon Hurricane`, intended for internal use only.<br>
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

`Storm::GetFloat::lognormal_variate(double log_mean, double log_deviation) -> double`<br>
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
- Hardware: 2.3 GHz 8-Core Intel Core i9, 16GB RAM, 1TB SSD
- Software: macOS Ventura 13.1, MonkeyTimer, Storm

```
MonkeyTimer
Storm Version: 3.7.0
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
percent_true(25.0): 16 nano
0: 75.162%
1: 24.838%

bernoulli_variate(0.25): 16 nano
0: 75.168%
1: 24.832%

Random Integer 
--------------
random_below(10): 33 nano
0: 10.173%
1: 9.931%
2: 9.89%
3: 10.045%
4: 10.025%
5: 10.098%
6: 9.924%
7: 9.934%
8: 9.951%
9: 10.029%

uniform_int_variate(0, 9): 29 nano
0: 10.02%
1: 9.978%
2: 9.99%
3: 9.847%
4: 9.993%
5: 9.98%
6: 9.998%
7: 10.139%
8: 10.063%
9: 9.992%

random_range(0, 20, 2): 42 nano
0: 10.158%
2: 10.028%
4: 9.897%
6: 10.04%
8: 9.858%
10: 9.885%
12: 10.122%
14: 10.004%
16: 9.945%
18: 10.063%

random_range(0, 20, -2): 44 nano
2: 9.926%
4: 10.11%
6: 9.974%
8: 9.99%
10: 9.968%
12: 9.955%
14: 9.957%
16: 10.038%
18: 10.064%
20: 10.018%

random_range(0, -20, 2): 42 nano
-20: 9.917%
-18: 9.906%
-16: 10.125%
-14: 10.14%
-12: 9.932%
-10: 9.968%
-8: 9.864%
-6: 10.022%
-4: 10.082%
-2: 10.044%

random_range(0, -20, -2): 42 nano
-18: 10.171%
-16: 9.99%
-14: 9.95%
-12: 10.026%
-10: 10.004%
-8: 10.003%
-6: 9.993%
-4: 9.983%
-2: 9.942%
0: 9.938%

plus_or_minus(10): 26 nano
-10: 4.706%
-9: 4.632%
-8: 4.763%
-7: 4.671%
-6: 4.857%
-5: 4.638%
-4: 4.839%
-3: 4.82%
-2: 4.741%
-1: 4.78%
0: 4.868%
1: 4.786%
2: 4.778%
3: 4.748%
4: 4.843%
5: 4.752%
6: 4.707%
7: 4.808%
8: 4.849%
9: 4.741%
10: 4.673%

plus_or_minus_linear(10): 53 nano
-10: 0.801%
-9: 1.647%
-8: 2.417%
-7: 3.353%
-6: 4.128%
-5: 4.817%
-4: 5.691%
-3: 6.628%
-2: 7.572%
-1: 8.233%
0: 9.073%
1: 8.214%
2: 7.466%
3: 6.734%
4: 5.84%
5: 5.041%
6: 4.1%
7: 3.287%
8: 2.448%
9: 1.686%
10: 0.824%

plus_or_minus_gauss(10): 60 nano
-10: 0.097%
-9: 0.224%
-8: 0.53%
-7: 1.149%
-6: 2.124%
-5: 3.595%
-4: 5.614%
-3: 7.956%
-2: 10.282%
-1: 11.888%
0: 12.659%
1: 11.872%
2: 10.286%
3: 7.987%
4: 5.769%
5: 3.651%
6: 2.237%
7: 1.122%
8: 0.597%
9: 0.255%
10: 0.106%

binomial_variate(10, 0.5): 88 nano
0: 0.087%
1: 0.965%
2: 4.448%
3: 11.902%
4: 20.555%
5: 24.515%
6: 20.131%
7: 11.78%
8: 4.512%
9: 1.002%
10: 0.103%

negative_binomial_variate(5, 0.75): 77 nano
0: 23.602%
1: 29.672%
2: 22.244%
3: 12.944%
4: 6.611%
5: 2.952%
6: 1.229%
7: 0.471%
8: 0.165%
9: 0.064%
10: 0.028%
11: 0.014%
12: 0.003%
14: 0.001%

geometric_variate(0.75): 23 nano
0: 74.993%
1: 18.776%
2: 4.689%
3: 1.135%
4: 0.307%
5: 0.068%
6: 0.024%
7: 0.007%
8: 0.001%

poisson_variate(4.0): 78 nano
0: 1.837%
1: 7.352%
2: 14.529%
3: 19.511%
4: 19.615%
5: 15.575%
6: 10.4%
7: 5.97%
8: 3.046%
9: 1.366%
10: 0.544%
11: 0.178%
12: 0.052%
13: 0.013%
14: 0.012%

Random Dice 
-----------
d(10): 27 nano
1: 10.217%
2: 9.996%
3: 9.982%
4: 10.083%
5: 9.867%
6: 9.938%
7: 10.044%
8: 9.962%
9: 9.967%
10: 9.944%

dice(3, 6): 76 nano
3: 0.442%
4: 1.459%
5: 2.721%
6: 4.684%
7: 6.927%
8: 9.829%
9: 11.554%
10: 12.431%
11: 12.5%
12: 11.775%
13: 9.63%
14: 6.817%
15: 4.586%
16: 2.748%
17: 1.445%
18: 0.452%

dice(0, 6): 1 nano
0: 100%

ability_dice(4): 151 nano
3: 0.077%
4: 0.311%
5: 0.727%
6: 1.586%
7: 2.839%
8: 4.847%
9: 7.131%
10: 9.411%
11: 11.343%
12: 12.923%
13: 13.179%
14: 12.476%
15: 10.142%
16: 7.291%
17: 4.112%
18: 1.605%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 35 nano
0: 9.856%
1: 10.08%
2: 10.042%
3: 9.969%
4: 9.973%
5: 10.019%
6: 10.003%
7: 10.119%
8: 10%
9: 9.939%

front_linear(N): 17 nano
0: 18.9%
1: 16.896%
2: 15.003%
3: 13.23%
4: 11.135%
5: 8.876%
6: 6.992%
7: 4.961%
8: 2.984%
9: 1.023%

middle_linear(N): 22 nano
0: 2.033%
1: 6.074%
2: 9.977%
3: 14.026%
4: 17.946%
5: 17.794%
6: 14.106%
7: 10.078%
8: 5.926%
9: 2.04%

back_linear(N): 16 nano
0: 0.98%
1: 3.057%
2: 5.103%
3: 6.948%
4: 8.917%
5: 11.07%
6: 12.969%
7: 14.859%
8: 17.016%
9: 19.081%

quantum_linear(N): 47 nano
0: 7.438%
1: 8.69%
2: 10.057%
3: 11.283%
4: 12.633%
5: 12.557%
6: 11.288%
7: 9.942%
8: 8.658%
9: 7.454%

front_gauss(N): 26 nano
0: 63.148%
1: 23.381%
2: 8.558%
3: 3.136%
4: 1.122%
5: 0.412%
6: 0.162%
7: 0.052%
8: 0.026%
9: 0.003%

middle_gauss(N): 56 nano
0: 0.004%
1: 0.135%
2: 2.067%
3: 13.615%
4: 34.287%
5: 34.182%
6: 13.473%
7: 2.135%
8: 0.101%
9: 0.001%

back_gauss(N): 29 nano
0: 0.005%
1: 0.022%
2: 0.053%
3: 0.152%
4: 0.421%
5: 1.18%
6: 3.115%
7: 8.58%
8: 23.207%
9: 63.265%

quantum_gauss(N): 68 nano
0: 20.943%
1: 7.793%
2: 3.664%
3: 5.688%
4: 11.901%
5: 12.108%
6: 5.627%
7: 3.6%
8: 7.863%
9: 20.813%

front_poisson(N): 75 nano
0: 8.146%
1: 20.593%
2: 25.858%
3: 21.379%
4: 13.137%
5: 6.648%
6: 2.808%
7: 0.972%
8: 0.354%
9: 0.105%

middle_poisson(N): 87 nano
0: 4.118%
1: 10.453%
2: 13.273%
3: 12.058%
4: 10.003%
5: 10.114%
6: 12.142%
7: 13.214%
8: 10.4%
9: 4.225%

back_poisson(N): 72 nano
0: 0.076%
1: 0.295%
2: 0.969%
3: 2.812%
4: 6.657%
5: 13.408%
6: 21.274%
7: 25.816%
8: 20.587%
9: 8.106%

quantum_poisson(N): 95 nano
0: 4.146%
1: 10.402%
2: 13.453%
3: 12.055%
4: 10.035%
5: 9.974%
6: 12.03%
7: 13.321%
8: 10.391%
9: 4.193%

quantum_monty(N): 93 nano
0: 10.885%
1: 8.973%
2: 8.89%
3: 9.705%
4: 11.559%
5: 11.513%
6: 9.54%
7: 9.074%
8: 8.89%
9: 10.971%

F(-N) where N = 10
random_index(-N): 35 nano
-10: 10.015%
-9: 10.131%
-8: 10.11%
-7: 10.035%
-6: 9.908%
-5: 10.114%
-4: 9.931%
-3: 10.087%
-2: 9.906%
-1: 9.763%

front_linear(-N): 17 nano
-10: 18.905%
-9: 17.095%
-8: 15.016%
-7: 12.727%
-6: 11.049%
-5: 9.148%
-4: 7.014%
-3: 4.983%
-2: 3.02%
-1: 1.043%

middle_linear(-N): 21 nano
-10: 2.021%
-9: 6.112%
-8: 9.954%
-7: 13.799%
-6: 17.971%
-5: 18.174%
-4: 14.07%
-3: 9.836%
-2: 6.002%
-1: 2.061%

back_linear(-N): 20 nano
-10: 0.972%
-9: 2.985%
-8: 4.901%
-7: 7.113%
-6: 9.186%
-5: 10.881%
-4: 12.858%
-3: 14.934%
-2: 17.002%
-1: 19.168%

quantum_linear(-N): 45 nano
-10: 7.421%
-9: 8.797%
-8: 9.965%
-7: 11.228%
-6: 12.576%
-5: 12.881%
-4: 11.158%
-3: 9.912%
-2: 8.637%
-1: 7.425%

front_gauss(-N): 27 nano
-10: 62.952%
-9: 23.406%
-8: 8.636%
-7: 3.116%
-6: 1.214%
-5: 0.415%
-4: 0.167%
-3: 0.056%
-2: 0.025%
-1: 0.013%

middle_gauss(-N): 60 nano
-10: 0.001%
-9: 0.114%
-8: 2.25%
-7: 13.569%
-6: 34.24%
-5: 34.042%
-4: 13.497%
-3: 2.144%
-2: 0.141%
-1: 0.002%

back_gauss(-N): 30 nano
-10: 0.011%
-9: 0.021%
-8: 0.061%
-7: 0.149%
-6: 0.418%
-5: 1.142%
-4: 3.19%
-3: 8.581%
-2: 23.314%
-1: 63.113%

quantum_gauss(-N): 76 nano
-10: 21.049%
-9: 7.664%
-8: 3.636%
-7: 5.658%
-6: 11.997%
-5: 11.832%
-4: 5.67%
-3: 3.511%
-2: 7.89%
-1: 21.093%

front_poisson(-N): 64 nano
-10: 8.396%
-9: 20.328%
-8: 25.557%
-7: 21.543%
-6: 13.414%
-5: 6.73%
-4: 2.707%
-3: 0.957%
-2: 0.296%
-1: 0.072%

middle_poisson(-N): 82 nano
-10: 4.152%
-9: 10.421%
-8: 13.234%
-7: 12.113%
-6: 10.08%
-5: 9.892%
-4: 12.293%
-3: 13.353%
-2: 10.275%
-1: 4.187%

back_poisson(-N): 63 nano
-10: 0.098%
-9: 0.326%
-8: 1.021%
-7: 2.777%
-6: 6.661%
-5: 13.232%
-4: 21.42%
-3: 25.854%
-2: 20.307%
-1: 8.304%

quantum_poisson(-N): 98 nano
-10: 4.193%
-9: 10.345%
-8: 13.487%
-7: 12.017%
-6: 10.086%
-5: 10.135%
-4: 12.072%
-3: 13.323%
-2: 10.237%
-1: 4.105%

quantum_monty(-N): 94 nano
-10: 10.802%
-9: 8.942%
-8: 9.136%
-7: 9.604%
-6: 11.443%
-5: 11.607%
-4: 9.73%
-3: 9.026%
-2: 8.937%
-1: 10.773%

Random Float: timer only 
------------------------
canonical_variate(): 13 nano
uniform_real_variate(1.0, 10.0): 14 nano
exponential_variate(1.0): 13 nano
gamma_variate(1.0, 1.0): 21 nano
weibull_variate(1.0, 1.0): 12 nano
normal_variate(5, 2): 34 nano
lognormal_variate(1.6, 0.25): 32 nano
extreme_value_variate(0.0, 1.0): 13 nano
chi_squared_variate(1.0): 74 nano
cauchy_variate(0.0, 0.00005): 15 nano
fisher_f_variate(1.0, 1.0): 143 nano
student_t_variate(1.0): 128 nano
beta_variate(10.0, 1.0): 92 nano
pareto_variate(5.0): 13 nano
vonmises_variate(1.0, 1.0): 99 nano
triangular_variate(0.0, 10.0, 5.0): 13 nano

Clamp Tests 
-----------
std::clamp(3, 2, 1): 0 nano
is 1 should be 2
clamp(3, 2, 1): 0 nano
is 2 should be 2

std::clamp(1.3, 1.2, 1.1): 0 nano
is 1.1 should be 1.2
clamp(1.3, 1.2, 1.1): 0 nano
is 1.2 should be 1.2

============================
Total Time: 0.616855 seconds
```
