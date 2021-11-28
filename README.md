#  Storm v3.5.3
### Random Number Toolkit
#### C++17 Header Only Library
#### Author: Robert Sharp
##### © 2021 Robert Sharp, all rights reserved.
<br>
<hr>
<br>

##### Project File List
- `Storm.hpp` complete core functionality
  - `GetBool` boolean variant namespace
  - `GetInt` long long variant namespace
  - `GetFloat` double variant namespace
- `MonkeyTimer.hpp` distribution/timer framework
- `StormTests.hpp` test implementation
- `Utilities.hpp` general utility tools, e.g. `print(...)`
- `main.cpp` executes a series of distribution and performance tests
- `README.md` this file
- `LICENSE` free and open for non-commercial use


Storm is intended to provide thread compatible, high-performance tools to make custom random number generators for Python3. 
Storm does not directly interact with the Python runtime and therefore requires additional tools like Cython to bridge the gap. 
Storm is optimized for multi-threaded execution on 64bit platforms.
<br><br>

#### Warning: Storm is not intended for secure hashing or encryption of any kind!
That said, Storm is far stronger than the vanilla MT implementation.<br><br>


Fortuna, RNG and Pyewacket are all built on Storm, each with its own set of goals. The RNG package features a 
C++ native approach, targeting python developers familiar with the C++ random library API. Pyewacket is a drop-in 
replacement for the python random library, for those that prefer the python random module API. The Fortuna module 
combines the features of Pyewacket and RNG. Fortuna also provides a collection of higher-level abstractions for 
creating custom random generators that can model any distribution across almost any dataset. Fortuna is fully generic 
and can be used for generating all value types including custom objects. Fortuna supports multi-dimensional data 
structures, nesting abstractions, automatic flattening, lazy evaluation and dependency injection. 

#### Python3 Extensions Based on Storm
Quick Install : `$ pip install Fortuna`<br>
Quick Install : `$ pip install RNG`<br>
Quick Install : `$ pip install Pyewacket`<br>

#### Distribution Range Notation:
- Inclusive Range: `[A, Z]` is the range A to Z including both A and Z.
- Exclusive Range: `(A, Z)` is the range A to Z excluding both A and Z.
- Tail Exclusion: `[A, Z)` is the range A to Z excluding Z.
- Head Exclusion: `(A, Z]` is the range A to Z excluding A.


---

## Core RNG Engine: Hurricane
`Storm::Engine::Hurricane`, intended for internal use only.<br>
Storm is powered by The Hurricane Engine, a customizable shuffle-drop configuration of the Mersenne Twister Algorithm. 
This shuffle-drop strategy makes Hurricane slightly less breakable than vanilla MTA. By default, Hurricane is configured 
to produce beautiful, hardware-seeded, sudo-random entropy at high-speed across multiple threads in parallel. Hurricane 
supports hardware entropy when available on the host platform, otherwise it will fall back to the system default.


## Random Generators

### Random Boolean Generator
`Storm::percent_true(double percent) -> bool`<br>
The input parameter represents the <u>percentage</u> of `true` in the distribution of the output. Input is clamped in 
range `[0.0, 100.0]`. Input that falls outside this range will saturate the boundaries without error.

```cpp
// percent_true.cpp
#include <iostream>
#include "Storm.hpp"

int main() {
    // 75% chance to print "Meow", 25% chance to print "Woof"
    if (Storm::GetBool::percent_true(75.0)) std::cout << "Meow" << "\n";
    else std::cout << "Woof" << "\n";
}
```

### Random Integer Generators
`Storm::GetInt::random_below(long long number) -> long long`<br>
Classic algorithm for generating a random number below a given value.<br>
Flat uniform distribution of the range `[0, number)`.

```cpp
// random_below.cpp
#include <iostream>
#include "Storm.hpp"

int main() {
    // prints a random number in range [0, 10)
    std::cout << Storm::GetInt::random_below(10) << "\n";
}
```

`Storm::GetInt::uniform_int_variate(long long a, long long b) -> long long`<br>
Flat uniform integer distribution in the range `[low, high]` where low = min(a, b) and high = max(b, a).

`Storm::GetInt::random_range(long long start, long long stop, long long step) -> long long`<br>
Where `low = min(start, stop)`, `high = max(start, stop)`<br>
Flat uniform distribution of the range `[low, high) by step`.<br>
Passing a negative `step` will invert the phase of the output distribution to `(low, high] by step`.<br>
`random_range(1, 10, 2) -> [1, 10) by 2`, odd numbers 1-9. Same as `random_range(10, 1, 2)`.<br>
`random_range(1, 10, -2) -> (1, 10] by -2`, even numbers 2-10. Same as `random_range(10, 1, -2)`.

`Storm::GetInt::plus_or_minus(long long number) -> long long`<br>
Flat uniform distribution in range `[-number, number]`, zero mean.

`Storm::GetInt::plus_or_minus_linear(long long number) -> long long`<br>
Linear distribution in range `[-number, number]`, zero mean.

`Storm::GetInt::plus_or_minus_gauss(long long number) -> long long`<br>
Gaussian distribution in range `[-number, number]`, zero mean.

`Storm::GetInt::binomial_variate(long long number_of_trials, double probability) -> long long`<br>
Binomial distribution based on number of trials and probability.

`Storm::GetInt::negative_binomial_variate(long long number_of_trials, double probability) -> long long`<br>
Negative binomial distribution based on number of trials and probability.

`Storm::GetInt::geometric_variate(double probability) -> long long`<br>
Geometric distribution based on probability.

`Storm::GetInt::poisson_variate(double mean) -> long long`<br>
Poisson distribution based on mean.


### Random Dice
`Storm::GetInt::d(long long sides) -> long long`<br>
Represents rolling a uniform multi-sided dice. `d(6)` is a regular six sided die.<br>
Flat uniform distribution of the range `[1, sides]`.<br>
By definition, `d(0)` will always return zero.<br>
Negative input produces the inverse output range. `d(-6) -> [-6, -1]`

`Storm::GetInt::dice(long long rolls, long long sides) -> long long`<br>
Computes the sum of multiple rolls of a multi-sided dice.<br>
`dice(3, 6)` is commonly written as 3d6 this is the same as a three six-sided dice rolled separately and added together.

`Storm::GetInt::ability_dice(long long number) -> long long`<br>
Input n is clamped in range `[3, 9]`<br>
Computes the sum of the top three of n `d(6)` rolls, often the input will be 4.<br>
Geometric distribution based on the number of six-sided dice rolled.<br>
Output will always be in range `[3, 18]`, the mean will increase with higher input values.


### ZeroCool Specification<br>Functors that produce random indices, specifically for Python3 lists.

ZeroCool Specification: All ZeroCool functors are pure functions with the following signature: `F(N) -> N` they take a 
long long N and return a random long long in range `[0, N)` and `[N, 0)` for negative N. Each ZeroCool functor models 
a unique distribution.

This specification is defined for the purpose of documenting the proper mapping of input to random output range such 
that - ZeroCool functors always produce valid python list indices that range from the front of a list for positive n, 
or the back for negative n, given |n| is less than or equal to the size of a non-empty list.

ZeroCool(0) is undefined: returning a sentinel, zero or negative one or raising an error or throwing an exception are 
all valid options as zero is a singularity in terms of distribution size. The following ZeroCool functors will return 
minus one for input zero, but this is not guaranteed or required by the spec. In most cases it simply never matters 
what a ZeroCool functor returns with zero for input, it could be indicative of an error but not nessesarily.

- #### Flat Uniform
`Storm::GetInt::random_index(long long) -> long long`: Flat Line<br>

- #### Linear Positional
`Storm::GetInt::front_linear(long long) -> long long`: Left Triangle<br>
`Storm::GetInt::middle_linear(long long) -> long long` Pyramid<br>
`Storm::GetInt::back_linear(long long) -> long long`: Right Triangle<br>
`Storm::GetInt::quantum_linear(long long) -> long long`: Sawtooth, 3 way Monty<br>

- #### Gaussian Positional
`Storm::GetInt::front_gauss(long long) -> long long`: Left Gamma<br>
`Storm::GetInt::middle_gauss(long long) -> long long`: Normal Gauss<br>
`Storm::GetInt::back_gauss(long long) -> long long`: Right Gamma<br>
`Storm::GetInt::quantum_gauss(long long) -> long long`: Gamma Wave, 3 way Monty<br>

- #### Poisson Positional
`Storm::GetInt::front_poisson(long long) -> long long`: Left Hill<br>
`Storm::GetInt::middle_poisson(long long) -> long long`: Twin Peaks<br>
`Storm::GetInt::back_poisson(long long) -> long long`: Right Hill<br>
`Storm::GetInt::quantum_poisson(long long) -> long long`: Poisson Wave, 3 way Monty<br>

- #### Quantum Monty
`Storm::GetInt::quantum_monty(long long) -> long long`: 3 by 3 way Monty<br>

### ZeroCool Python Examples

#### Produce a front linear distribution of the last five values of a list
```
# front_linear.py
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
# back_linear.py
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
# middle_linear.py
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


### Random Floating Point
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
- Hardware: Intel Core i9-9880H, 16GB RAM, 1TB SSD
- Software: macOS Big Sur 11.6.1, MonkeyTimer, Storm
- Compile  `$ clang++ main.cpp -std=c++17 -Ofast -march=native -o stormtests.o`
- Run Tests: `$ ./stormtests.o`

```
MonkeyTimer
Storm Version: 3.5.3
===========

Min/Max Tests 
-------------
Minimum Integer: -9223372036854775807
Maximum Integer:  9223372036854775807
Minimum Float: -1.79769e+308
Maximum Float:  1.79769e+308
Minimum Below: -4.94066e-324
Minimum Above:  4.94066e-324

Random Boolean 
--------------
percent_true(25.0): 22 nano
0: 75.162%
1: 24.838%

bernoulli_variate(0.25): 19 nano
0: 74.978%
1: 25.022%

Random Integer 
--------------
random_below(10): 42 nano
0: 9.943%
1: 10.176%
2: 9.916%
3: 9.996%
4: 9.87%
5: 10.008%
6: 9.997%
7: 10.016%
8: 10.005%
9: 10.073%

uniform_int_variate(0, 9): 38 nano
0: 10.011%
1: 9.976%
2: 10.007%
3: 9.932%
4: 10.085%
5: 9.951%
6: 10.216%
7: 9.902%
8: 9.961%
9: 9.959%

random_range(0, 20, 2): 45 nano
0: 9.947%
2: 9.998%
4: 10.055%
6: 10.048%
8: 9.868%
10: 9.965%
12: 9.982%
14: 9.956%
16: 10.027%
18: 10.154%

random_range(0, 20, -2): 49 nano
2: 9.923%
4: 9.985%
6: 9.97%
8: 9.954%
10: 10.069%
12: 10.015%
14: 10.03%
16: 9.915%
18: 10.035%
20: 10.104%

random_range(0, -20, 2): 40 nano
-20: 10.08%
-18: 10.112%
-16: 9.855%
-14: 10.035%
-12: 9.967%
-10: 9.888%
-8: 9.988%
-6: 9.957%
-4: 9.961%
-2: 10.157%

random_range(0, -20, -2): 47 nano
-18: 10.051%
-16: 10.106%
-14: 9.906%
-12: 10.104%
-10: 10.094%
-8: 10.115%
-6: 9.726%
-4: 9.934%
-2: 9.984%
0: 9.98%

plus_or_minus(10): 24 nano
-10: 4.772%
-9: 4.751%
-8: 4.739%
-7: 4.729%
-6: 4.849%
-5: 4.656%
-4: 4.784%
-3: 4.836%
-2: 4.762%
-1: 4.821%
0: 4.73%
1: 4.716%
2: 4.724%
3: 4.703%
4: 4.805%
5: 4.764%
6: 4.756%
7: 4.829%
8: 4.861%
9: 4.714%
10: 4.699%

plus_or_minus_linear(10): 49 nano
-10: 0.789%
-9: 1.648%
-8: 2.5%
-7: 3.326%
-6: 4.014%
-5: 4.937%
-4: 5.877%
-3: 6.544%
-2: 7.445%
-1: 8.325%
0: 9.197%
1: 8.154%
2: 7.536%
3: 6.55%
4: 5.867%
5: 4.935%
6: 4.125%
7: 3.373%
8: 2.45%
9: 1.631%
10: 0.777%

plus_or_minus_gauss(10): 52 nano
-10: 0.079%
-9: 0.232%
-8: 0.529%
-7: 1.196%
-6: 2.106%
-5: 3.664%
-4: 5.831%
-3: 7.971%
-2: 10.311%
-1: 11.833%
0: 12.464%
1: 12.009%
2: 10.177%
3: 8.108%
4: 5.746%
5: 3.723%
6: 2.04%
7: 1.169%
8: 0.52%
9: 0.222%
10: 0.07%

binomial_variate(10, 0.5): 80 nano
0: 0.103%
1: 1.013%
2: 4.405%
3: 11.694%
4: 20.586%
5: 24.657%
6: 20.383%
7: 11.619%
8: 4.381%
9: 1.054%
10: 0.105%

negative_binomial_variate(5, 0.75): 70 nano
0: 23.72%
1: 29.537%
2: 22.535%
3: 12.818%
4: 6.522%
5: 2.817%
6: 1.252%
7: 0.507%
8: 0.174%
9: 0.083%
10: 0.025%
11: 0.008%
12: 0.001%
13: 0.001%

geometric_variate(0.75): 24 nano
0: 75.167%
1: 18.552%
2: 4.651%
3: 1.222%
4: 0.31%
5: 0.077%
6: 0.014%
7: 0.007%

poisson_variate(4.0): 65 nano
0: 1.777%
1: 7.267%
2: 14.449%
3: 19.88%
4: 19.473%
5: 15.732%
6: 10.44%
7: 5.823%
8: 3.061%
9: 1.347%
10: 0.496%
11: 0.169%
12: 0.065%
13: 0.018%
14: 0.003%

Random Dice 
-----------
d(10): 24 nano
1: 9.864%
2: 9.997%
3: 10.056%
4: 10.081%
5: 9.928%
6: 10.072%
7: 9.99%
8: 10.001%
9: 9.999%
10: 10.012%

dice(3, 6): 64 nano
3: 0.523%
4: 1.364%
5: 2.77%
6: 4.637%
7: 6.862%
8: 9.687%
9: 11.65%
10: 12.528%
11: 12.645%
12: 11.57%
13: 9.698%
14: 6.858%
15: 4.67%
16: 2.73%
17: 1.362%
18: 0.446%

dice(0, 6): 4 nano
0: 100%

ability_dice(4): 151 nano
3: 0.069%
4: 0.33%
5: 0.736%
6: 1.626%
7: 2.999%
8: 4.797%
9: 7.047%
10: 9.241%
11: 11.427%
12: 12.891%
13: 13.211%
14: 12.45%
15: 10.005%
16: 7.44%
17: 4.14%
18: 1.591%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 32 nano
0: 9.939%
1: 10.105%
2: 9.995%
3: 10.126%
4: 9.946%
5: 9.909%
6: 10.046%
7: 10.076%
8: 9.993%
9: 9.865%

front_linear(N): 21 nano
0: 19.059%
1: 17.068%
2: 14.878%
3: 13.072%
4: 11.033%
5: 8.844%
6: 7.076%
7: 4.962%
8: 2.974%
9: 1.034%

middle_linear(N): 20 nano
0: 2.084%
1: 5.991%
2: 10.107%
3: 14.06%
4: 18.066%
5: 18.052%
6: 13.845%
7: 9.839%
8: 5.99%
9: 1.966%

back_linear(N): 16 nano
0: 0.958%
1: 2.972%
2: 4.922%
3: 6.952%
4: 9.037%
5: 11.032%
6: 13.112%
7: 15.03%
8: 17.044%
9: 18.941%

quantum_linear(N): 56 nano
0: 7.369%
1: 8.611%
2: 10.211%
3: 11.351%
4: 12.658%
5: 12.542%
6: 11.27%
7: 9.984%
8: 8.667%
9: 7.337%

front_gauss(N): 23 nano
0: 63.213%
1: 23.218%
2: 8.592%
3: 3.123%
4: 1.159%
5: 0.454%
6: 0.147%
7: 0.058%
8: 0.028%
9: 0.008%

middle_gauss(N): 60 nano
0: 0.003%
1: 0.117%
2: 2.209%
3: 13.546%
4: 34.037%
5: 34.335%
6: 13.526%
7: 2.09%
8: 0.129%
9: 0.008%

back_gauss(N): 26 nano
0: 0.008%
1: 0.019%
2: 0.07%
3: 0.16%
4: 0.447%
5: 1.16%
6: 3.195%
7: 8.627%
8: 23.205%
9: 63.109%

quantum_gauss(N): 55 nano
0: 21.13%
1: 7.822%
2: 3.585%
3: 5.574%
4: 11.931%
5: 11.968%
6: 5.482%
7: 3.644%
8: 7.808%
9: 21.056%

front_poisson(N): 63 nano
0: 8.123%
1: 20.641%
2: 25.728%
3: 21.35%
4: 13.317%
5: 6.594%
6: 2.822%
7: 1.02%
8: 0.326%
9: 0.079%

middle_poisson(N): 93 nano
0: 4.158%
1: 10.492%
2: 13.333%
3: 12.112%
4: 9.914%
5: 10.055%
6: 12.073%
7: 13.213%
8: 10.44%
9: 4.21%

back_poisson(N): 66 nano
0: 0.096%
1: 0.274%
2: 0.969%
3: 2.757%
4: 6.68%
5: 13.498%
6: 21.053%
7: 25.814%
8: 20.612%
9: 8.247%

quantum_poisson(N): 92 nano
0: 4.116%
1: 10.434%
2: 13.388%
3: 12.257%
4: 10.059%
5: 10.13%
6: 11.981%
7: 13.246%
8: 10.313%
9: 4.076%

quantum_monty(N): 83 nano
0: 10.84%
1: 9.008%
2: 8.993%
3: 9.701%
4: 11.642%
5: 11.493%
6: 9.62%
7: 8.903%
8: 8.981%
9: 10.819%

F(-N) where N = 10
random_index(-N): 30 nano
-10: 10.035%
-9: 10.132%
-8: 10.036%
-7: 9.93%
-6: 10.019%
-5: 9.9%
-4: 9.932%
-3: 10.012%
-2: 10.05%
-1: 9.954%

front_linear(-N): 15 nano
-10: 18.945%
-9: 17.007%
-8: 14.972%
-7: 12.972%
-6: 11.187%
-5: 8.91%
-4: 7.014%
-3: 4.939%
-2: 3.051%
-1: 1.003%

middle_linear(-N): 20 nano
-10: 2.025%
-9: 6.013%
-8: 10.04%
-7: 13.929%
-6: 18.003%
-5: 18.098%
-4: 13.976%
-3: 9.835%
-2: 6.14%
-1: 1.941%

back_linear(-N): 17 nano
-10: 1.007%
-9: 3.023%
-8: 4.999%
-7: 6.96%
-6: 8.993%
-5: 10.879%
-4: 12.931%
-3: 14.871%
-2: 17.077%
-1: 19.26%

quantum_linear(-N): 40 nano
-10: 7.359%
-9: 8.631%
-8: 9.923%
-7: 11.365%
-6: 12.68%
-5: 12.821%
-4: 11.261%
-3: 10.081%
-2: 8.55%
-1: 7.329%

front_gauss(-N): 24 nano
-10: 63.024%
-9: 23.377%
-8: 8.483%
-7: 3.243%
-6: 1.207%
-5: 0.437%
-4: 0.139%
-3: 0.06%
-2: 0.019%
-1: 0.011%

middle_gauss(-N): 54 nano
-10: 0.001%
-9: 0.151%
-8: 2.165%
-7: 13.658%
-6: 34.288%
-5: 33.979%
-4: 13.48%
-3: 2.143%
-2: 0.133%
-1: 0.002%

back_gauss(-N): 26 nano
-10: 0.007%
-9: 0.024%
-8: 0.057%
-7: 0.152%
-6: 0.454%
-5: 1.176%
-4: 3.187%
-3: 8.514%
-2: 23.399%
-1: 63.03%

quantum_gauss(-N): 58 nano
-10: 20.988%
-9: 7.762%
-8: 3.635%
-7: 5.825%
-6: 11.867%
-5: 11.926%
-4: 5.633%
-3: 3.528%
-2: 7.84%
-1: 20.996%

front_poisson(-N): 60 nano
-10: 8.229%
-9: 20.361%
-8: 25.681%
-7: 21.336%
-6: 13.457%
-5: 6.636%
-4: 2.869%
-3: 0.979%
-2: 0.353%
-1: 0.099%

middle_poisson(-N): 88 nano
-10: 4.136%
-9: 10.466%
-8: 13.357%
-7: 12.082%
-6: 10.231%
-5: 10.063%
-4: 11.881%
-3: 13.278%
-2: 10.348%
-1: 4.158%

back_poisson(-N): 71 nano
-10: 0.076%
-9: 0.295%
-8: 0.982%
-7: 2.749%
-6: 6.635%
-5: 13.578%
-4: 21.383%
-3: 25.493%
-2: 20.717%
-1: 8.092%

quantum_poisson(-N): 95 nano
-10: 4.139%
-9: 10.319%
-8: 13.323%
-7: 12.036%
-6: 9.965%
-5: 9.978%
-4: 12.244%
-3: 13.364%
-2: 10.428%
-1: 4.204%

quantum_monty(-N): 94 nano
-10: 10.912%
-9: 8.977%
-8: 8.88%
-7: 9.948%
-6: 11.503%
-5: 11.483%
-4: 9.584%
-3: 9.009%
-2: 8.964%
-1: 10.74%

Random Float: timer only 
------------------------
canonical_variate(): 13 nano
uniform_real_variate(1.0, 10.0): 12 nano
exponential_variate(1.0): 11 nano
gamma_variate(1.0, 1.0): 20 nano
weibull_variate(1.0, 1.0): 11 nano
normal_variate(5, 2): 31 nano
lognormal_variate(1.6, 0.25): 31 nano
extreme_value_variate(0.0, 1.0): 11 nano
chi_squared_variate(1.0): 66 nano
cauchy_variate(0.0, 0.00005): 11 nano
fisher_f_variate(1.0, 1.0): 127 nano
student_t_variate(1.0): 117 nano
beta_variate(10.0, 1.0): 85 nano
pareto_variate(5.0): 11 nano
vonmises_variate(1.0, 1.0): 91 nano
triangular_variate(0.0, 10.0, 5.0): 11 nano

Clamp Tests 
-----------
clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano

============================
Total Time: 0.58043 seconds

```
