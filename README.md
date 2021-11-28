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
- Software: macOS Big Sur 11.5.2, MonkeyTimer, Storm
- Compiler  `$ clang++ main.cpp -std=c++17 -Ofast -march=native -o stormtests.o`
- Run Tests: `$ ./stormtests.o`

```
MonkeyTimer
Storm Version 3.5.2
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
percent_true(25.0): 17 nano
0: 75.2%
1: 24.8%

bernoulli_variate(0.25): 16 nano
0: 75.109%
1: 24.891%

Random Integer 
--------------
random_below(10): 36 nano
0: 10.002%
1: 9.877%
2: 9.897%
3: 9.965%
4: 10.142%
5: 10.098%
6: 10.178%
7: 9.847%
8: 9.975%
9: 10.019%

uniform_int_variate(0, 9): 32 nano
0: 9.909%
1: 9.953%
2: 10.222%
3: 9.775%
4: 10.189%
5: 9.967%
6: 10.129%
7: 10.044%
8: 9.977%
9: 9.835%

random_range(0, 20, 2): 38 nano
0: 10.084%
2: 9.982%
4: 10.147%
6: 9.889%
8: 10.07%
10: 9.795%
12: 9.947%
14: 10.087%
16: 9.91%
18: 10.089%

random_range(0, 20, -2): 43 nano
2: 10.081%
4: 9.905%
6: 9.928%
8: 10.13%
10: 9.934%
12: 10.081%
14: 9.914%
16: 10.13%
18: 10.003%
20: 9.894%

random_range(0, -20, 2): 39 nano
-20: 10.067%
-18: 9.961%
-16: 9.968%
-14: 10.194%
-12: 9.897%
-10: 9.958%
-8: 10.113%
-6: 9.969%
-4: 9.934%
-2: 9.939%

random_range(0, -20, -2): 42 nano
-18: 9.977%
-16: 10.066%
-14: 10.036%
-12: 9.966%
-10: 10.117%
-8: 9.895%
-6: 10.11%
-4: 9.974%
-2: 9.877%
0: 9.982%

plus_or_minus(10): 25 nano
-10: 4.761%
-9: 4.711%
-8: 4.86%
-7: 4.815%
-6: 4.845%
-5: 4.782%
-4: 4.797%
-3: 4.789%
-2: 4.831%
-1: 4.711%
0: 4.747%
1: 4.782%
2: 4.68%
3: 4.707%
4: 4.756%
5: 4.725%
6: 4.717%
7: 4.784%
8: 4.76%
9: 4.718%
10: 4.722%

plus_or_minus_linear(10): 50 nano
-10: 0.821%
-9: 1.634%
-8: 2.492%
-7: 3.39%
-6: 4.225%
-5: 4.854%
-4: 5.801%
-3: 6.615%
-2: 7.462%
-1: 8.149%
0: 9.094%
1: 8.23%
2: 7.335%
3: 6.624%
4: 5.904%
5: 4.935%
6: 4.159%
7: 3.264%
8: 2.482%
9: 1.688%
10: 0.842%

plus_or_minus_gauss(10): 57 nano
-10: 0.093%
-9: 0.216%
-8: 0.572%
-7: 1.113%
-6: 2.128%
-5: 3.724%
-4: 5.701%
-3: 8.123%
-2: 10.213%
-1: 11.846%
0: 12.49%
1: 12.004%
2: 10.268%
3: 7.967%
4: 5.67%
5: 3.681%
6: 2.218%
7: 1.114%
8: 0.546%
9: 0.239%
10: 0.074%

binomial_variate(10, 0.5): 86 nano
0: 0.097%
1: 0.997%
2: 4.453%
3: 11.894%
4: 20.527%
5: 24.578%
6: 20.32%
7: 11.721%
8: 4.392%
9: 0.935%
10: 0.086%

negative_binomial_variate(5, 0.75): 74 nano
0: 23.98%
1: 29.694%
2: 21.913%
3: 12.924%
4: 6.508%
5: 2.987%
6: 1.188%
7: 0.53%
8: 0.173%
9: 0.068%
10: 0.026%
11: 0.006%
12: 0.001%
13: 0.002%

geometric_variate(0.75): 26 nano
0: 74.985%
1: 18.928%
2: 4.498%
3: 1.192%
4: 0.301%
5: 0.067%
6: 0.024%
7: 0.004%
8: 0.001%

poisson_variate(4.0): 84 nano
0: 1.776%
1: 7.305%
2: 14.687%
3: 19.711%
4: 19.434%
5: 15.619%
6: 10.417%
7: 5.929%
8: 2.939%
9: 1.308%
10: 0.588%
11: 0.189%
12: 0.073%
13: 0.02%
14: 0.005%

Random Dice 
-----------
d(10): 28 nano
1: 10.037%
2: 10.137%
3: 10.083%
4: 9.935%
5: 9.937%
6: 10.004%
7: 9.925%
8: 10.024%
9: 9.986%
10: 9.932%

dice(3, 6): 79 nano
3: 0.507%
4: 1.454%
5: 2.807%
6: 4.621%
7: 6.813%
8: 9.672%
9: 11.521%
10: 12.547%
11: 12.438%
12: 11.541%
13: 9.867%
14: 6.967%
15: 4.744%
16: 2.716%
17: 1.36%
18: 0.425%

dice(0, 6): 2 nano
0: 100%

ability_dice(4): 163 nano
3: 0.081%
4: 0.34%
5: 0.789%
6: 1.575%
7: 3.076%
8: 4.873%
9: 6.983%
10: 9.355%
11: 11.397%
12: 12.907%
13: 13.207%
14: 12.182%
15: 10.1%
16: 7.27%
17: 4.214%
18: 1.651%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 30 nano
0: 9.929%
1: 10.056%
2: 9.901%
3: 10.119%
4: 10.035%
5: 9.928%
6: 9.893%
7: 10.131%
8: 10.13%
9: 9.878%

front_linear(N): 17 nano
0: 18.923%
1: 17.094%
2: 14.91%
3: 12.974%
4: 10.918%
5: 9.053%
6: 7.053%
7: 5.035%
8: 3.01%
9: 1.03%

middle_linear(N): 33 nano
0: 1.985%
1: 6.084%
2: 9.844%
3: 13.984%
4: 18.155%
5: 17.945%
6: 14.09%
7: 9.956%
8: 5.988%
9: 1.969%

back_linear(N): 18 nano
0: 0.996%
1: 3.006%
2: 4.969%
3: 6.897%
4: 8.954%
5: 11.105%
6: 13.012%
7: 14.988%
8: 16.867%
9: 19.206%

quantum_linear(N): 46 nano
0: 7.396%
1: 8.624%
2: 9.974%
3: 11.227%
4: 12.669%
5: 12.634%
6: 11.369%
7: 9.952%
8: 8.775%
9: 7.38%

front_gauss(N): 24 nano
0: 62.978%
1: 23.298%
2: 8.8%
3: 3.144%
4: 1.133%
5: 0.414%
6: 0.141%
7: 0.066%
8: 0.018%
9: 0.008%

middle_gauss(N): 58 nano
0: 0.005%
1: 0.113%
2: 2.155%
3: 13.589%
4: 34.233%
5: 34.007%
6: 13.602%
7: 2.168%
8: 0.126%
9: 0.002%

back_gauss(N): 25 nano
0: 0.01%
1: 0.023%
2: 0.054%
3: 0.15%
4: 0.389%
5: 1.153%
6: 3.067%
7: 8.528%
8: 23.194%
9: 63.432%

quantum_gauss(N): 68 nano
0: 21.085%
1: 7.815%
2: 3.646%
3: 5.637%
4: 11.769%
5: 11.849%
6: 5.6%
7: 3.492%
8: 7.799%
9: 21.308%

front_poisson(N): 65 nano
0: 8.326%
1: 20.629%
2: 25.479%
3: 21.384%
4: 13.346%
5: 6.749%
6: 2.749%
7: 0.952%
8: 0.304%
9: 0.082%

middle_poisson(N): 81 nano
0: 4.15%
1: 10.4%
2: 13.375%
3: 12.033%
4: 10.151%
5: 9.979%
6: 12.023%
7: 13.246%
8: 10.465%
9: 4.178%

back_poisson(N): 59 nano
0: 0.092%
1: 0.3%
2: 1.03%
3: 2.796%
4: 6.728%
5: 13.454%
6: 21.391%
7: 25.47%
8: 20.574%
9: 8.165%

quantum_poisson(N): 91 nano
0: 4.046%
1: 10.454%
2: 13.413%
3: 12.068%
4: 9.888%
5: 9.966%
6: 12.058%
7: 13.483%
8: 10.45%
9: 4.174%

quantum_monty(N): 85 nano
0: 10.931%
1: 8.997%
2: 8.852%
3: 9.655%
4: 11.456%
5: 11.483%
6: 9.86%
7: 8.903%
8: 8.813%
9: 11.05%

F(-N) where N = 10
random_index(-N): 30 nano
-10: 9.791%
-9: 10.133%
-8: 9.808%
-7: 9.984%
-6: 10.177%
-5: 10.115%
-4: 9.998%
-3: 10.02%
-2: 10.147%
-1: 9.827%

front_linear(-N): 19 nano
-10: 18.83%
-9: 16.909%
-8: 15.127%
-7: 12.923%
-6: 11.085%
-5: 9.061%
-4: 6.96%
-3: 5.087%
-2: 3.027%
-1: 0.991%

middle_linear(-N): 21 nano
-10: 2.04%
-9: 6.027%
-8: 10.066%
-7: 14.128%
-6: 17.919%
-5: 17.981%
-4: 13.911%
-3: 9.907%
-2: 6.057%
-1: 1.964%

back_linear(-N): 18 nano
-10: 0.967%
-9: 3.013%
-8: 4.991%
-7: 7.056%
-6: 9.027%
-5: 10.927%
-4: 12.951%
-3: 14.994%
-2: 16.949%
-1: 19.125%

quantum_linear(-N): 42 nano
-10: 7.267%
-9: 8.646%
-8: 9.986%
-7: 11.381%
-6: 12.673%
-5: 12.728%
-4: 11.351%
-3: 9.894%
-2: 8.688%
-1: 7.386%

front_gauss(-N): 26 nano
-10: 63.202%
-9: 23.315%
-8: 8.627%
-7: 3.019%
-6: 1.212%
-5: 0.389%
-4: 0.146%
-3: 0.057%
-2: 0.022%
-1: 0.011%

middle_gauss(-N): 57 nano
-10: 0.001%
-9: 0.132%
-8: 2.135%
-7: 13.595%
-6: 34.577%
-5: 33.682%
-4: 13.683%
-3: 2.057%
-2: 0.137%
-1: 0.001%

back_gauss(-N): 26 nano
-10: 0.009%
-9: 0.025%
-8: 0.068%
-7: 0.152%
-6: 0.417%
-5: 1.126%
-4: 3.143%
-3: 8.448%
-2: 23.217%
-1: 63.395%

quantum_gauss(-N): 58 nano
-10: 21.121%
-9: 7.815%
-8: 3.652%
-7: 5.599%
-6: 11.816%
-5: 11.981%
-4: 5.576%
-3: 3.573%
-2: 7.83%
-1: 21.037%

front_poisson(-N): 59 nano
-10: 8.305%
-9: 20.389%
-8: 25.787%
-7: 21.275%
-6: 13.289%
-5: 6.731%
-4: 2.812%
-3: 1.008%
-2: 0.314%
-1: 0.09%

middle_poisson(-N): 80 nano
-10: 4.156%
-9: 10.495%
-8: 13.105%
-7: 12.043%
-6: 10.004%
-5: 10.078%
-4: 12.123%
-3: 13.264%
-2: 10.525%
-1: 4.207%

back_poisson(-N): 74 nano
-10: 0.077%
-9: 0.315%
-8: 0.971%
-7: 2.773%
-6: 6.668%
-5: 13.288%
-4: 21.29%
-3: 25.833%
-2: 20.587%
-1: 8.198%

quantum_poisson(-N): 94 nano
-10: 4.09%
-9: 10.364%
-8: 13.326%
-7: 12.147%
-6: 9.968%
-5: 10.033%
-4: 12.065%
-3: 13.45%
-2: 10.439%
-1: 4.118%

quantum_monty(-N): 91 nano
-10: 10.784%
-9: 9.093%
-8: 8.717%
-7: 9.854%
-6: 11.544%
-5: 11.466%
-4: 9.746%
-3: 9.056%
-2: 8.834%
-1: 10.906%

Random Float: timer only 
------------------------
canonical_variate(): 11 nano
uniform_real_variate(1.0, 10.0): 11 nano
exponential_variate(1.0): 11 nano
gamma_variate(1.0, 1.0): 20 nano
weibull_variate(1.0, 1.0): 12 nano
normal_variate(5, 2): 32 nano
lognormal_variate(1.6, 0.25): 32 nano
extreme_value_variate(0.0, 1.0): 12 nano
chi_squared_variate(1.0): 70 nano
cauchy_variate(0.0, 0.00005): 11 nano
fisher_f_variate(1.0, 1.0): 136 nano
student_t_variate(1.0): 133 nano
beta_variate(10.0, 1.0): 92 nano
pareto_variate(5.0): 11 nano
vonmises_variate(1.0, 1.0): 101 nano
triangular_variate(0.0, 10.0, 5.0): 12 nano

Clamp Tests 
-----------
clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano

============================
Total Time: 0.598563 seconds

```
