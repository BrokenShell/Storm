#  Storm v3.4.2
### Random Number Toolkit
#### C++17 Header Only Library
#### Author: Robert Sharp
##### © 2020 Robert Sharp, all rights reserved.
<br>
<hr>
<br>

##### Project File List
- `Storm.hpp` complete core functionality.
-  `MonkeyTimer.hpp` distribution/timer framework.
- `StormTests.hpp` test implementation.
- `main.cpp` executes a series of tests.
- `README.md` this file.
- `LICENSE` free and open for non-commercial use. Other licenses may be available, contact the author for details.


Storm is intended to provide thread compatible low-level tools to make high-performance random number generators for Python3. Storm does not directly interact with the Python runtime and requires additional tools like Cython to bridge the gap. Storm is optimized for multi-threaded execution on 64bit platforms.

#### Warning: Storm is not intended for secure hashing or encryption of any kind!<br><br>

Fortuna, RNG and Pyewacket are all built on Storm, each with its own set of goals. The RNG package features a C++ native approach, targeting python developers familiar with the C++ random library API. Pyewacket is a drop-in replacement for the python random library, for those that prefer the python random module API. The Fortuna module combines the best of both and builds on top of them a collection of abstractions for creating random generators that can model any distribution. Fortuna is fully generic and can be used for generating all value types including custom objects. Fortuna supports multi-dimensional data structures, nesting abstractions, automatic flattening, lazy evaluation and dependency injection. 

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

## RNG Engine: Hurricane
`Storm::Engine::Hurricane`, intended for internal use only.<br>
Storm is powered by The Hurricane Engine, a customizable shuffle-drop configuration of the Mersenne Twister Algorithm. This shuffle-drop strategy makes Hurricane slightly less breakable than vanilla MTA. By default, Hurricane is configured to produce beautiful, hardware-seeded, sudo-random entropy at high-speed across multiple threads. Hurricane supports hardware entropy when available on the host platform, otherwise it will fallback to the system default.


## Random Generators

### Random Boolean Generator
`Storm::percent_true(double percent) -> bool`<br>
The input parameter represents the <u>percentage</u> of `true` in the distribution of the output. Input is clamped in range `[0.0, 100.0]`. Input that falls outside this range will saturate the boundaries without error.

```cpp
// percent_true.cpp
#include <iostream>
#include "Storm.hpp"

int main() {
    // 75% chance to print "Meow", 25% chance to print "Woof"
    if (Storm::percent_true(75.0)) std::cout << "Meow" << "\n";
    else std::cout << "Woof" << "\n";
}
```

### Random Integer Generators
`Storm::random_below(long long number) -> long long`<br>
Classic algorithm for finding a random number below a given value.<br>
Flat uniform distribution of the range `[0, number)`.

```cpp
// random_below.cpp
#include <iostream>
#include "Storm.hpp"

int main() {
    // prints a random number in range [0, 10)
    std::cout << Storm::random_below(10) << "\n";
}
```

`Storm::uniform_int_variate(long long a, long long b) -> long long`<br>
Flat uniform integer distribution in the range `[low, high]` where low = min(a, b) and high = max(b, a).

`Storm::random_range(long long start, long long stop, long long step) -> long long`<br>
Where `low = min(start, stop)`, `high = max(start, stop)`<br>
Flat uniform distribution of the range `[low, high) by step`.<br>
Passing a negative `step` will invert the phase of the output distribution to `(low, high] by step`.<br>
`random_range(1, 10, 2) -> [1, 10) by 2`, odd numbers 1-9. Same as `random_range(10, 1, 2)`.<br>
`random_range(1, 10, -2) -> (1, 10] by -2`, even numbers 2-10. Same as `random_range(10, 1, -2)`.

`Storm::plus_or_minus(long long number) -> long long`<br>
Flat uniform distribution in range `[-number, number]`, zero mean.

`Storm::plus_or_minus_linear(long long number) -> long long`<br>
Linear distribution in range `[-number, number]`, zero mean.

`Storm::plus_or_minus_gauss(long long number) -> long long`<br>
Gaussian distribution in range `[-number, number]`, zero mean.

`Storm::binomial_variate(long long number_of_trials, double probability) -> long long`<br>
Binomial distribution based on number of trials and probability.

`Storm::negative_binomial_variate(long long number_of_trials, double probability) -> long long`<br>
Negative binomial distribution based on number of trials and probability.

`Storm::geometric_variate(double probability) -> long long`<br>
Geometric distribution based on probability.

`Storm::poisson_variate(double mean) -> long long`<br>
Poisson distribution based on mean.


### Random Dice
`Storm::d(long long sides) -> long long`<br>
Represents rolling a uniform multi-sided dice. `d(6)` is a regular six sided die.<br>
Flat uniform distribution of the range `[1, sides]`.<br>
By definition, `d(0)` will always return zero.<br>
Negative input produces the inverse output range. `d(-6) -> [-6, -1]`

`Storm::dice(long long rolls, long long sides) -> long long`<br>
Computes the sum of multiple rolls of a multi-sided dice.<br>
`dice(3, 6)` is commonly written as 3d6 this is the same as a three six-sided dice rolled separately and added together.

`Storm::ability_dice(long long number) -> long long`<br>
Input n is clamped in range `[3, 9]`<br>
Computes the sum of the top three of n `d(6)` rolls, often the input will be 4.<br>
Geometric distribution based on the number of six-sided dice rolled.<br>
Output will always be in range `[3, 18]`, the mean will increase with higher input values.


### ZeroCool Specification<br>Functors that produce random indices, specifically for Python3 lists.

ZeroCool Specification: All ZeroCool functors are pure functions with the following signature: `F(N) -> N` they take a long long N and return a random long long in range `[0, N)` and `[N, 0)` for negative N. Each ZeroCool functor models a unique distribution.

This specification is defined for the purpose of documenting the proper mapping of input to random output range such that - ZeroCool functors always produce valid python list indices that range from the front of a list for positive n, or the back for negative n, given |n| is less than or equal to the size of a non-empty list.

ZeroCool(0) is undefined: returning a sentinel, zero or negative one or raising an error or throwing an exception are all valid options as zero is a singularity in terms of distribution size. The following ZeroCool functors will return minus one for input zero, but this is not guaranteed or required by the spec. In most cases it simply never matters what a ZeroCool functor returns with zero for input, it could be indicative of an error but not nessesarily.

- #### Flat Uniform
`Storm::random_index(long long) -> long long`: Flat Line<br>

- #### Linear Positional
`Storm::front_linear(long long) -> long long`: Left Triangle<br>
`Storm::middle_linear(long long) -> long long` Pyramid<br>
`Storm::back_linear(long long) -> long long`: Right Triangle<br>
`Storm::quantum_linear(long long) -> long long`: Sawtooth, 3 way Monty<br>

- #### Gaussian Positional
`Storm::front_gauss(long long) -> long long`: Left Gamma<br>
`Storm::middle_gauss(long long) -> long long`: Normal Gauss<br>
`Storm::back_gauss(long long) -> long long`: Right Gamma<br>
`Storm::quantum_gauss(long long) -> long long`: Gamma Wave, 3 way Monty<br>

- #### Poisson Positional
`Storm::front_poisson(long long) -> long long`: Left Hill<br>
`Storm::middle_poisson(long long) -> long long`: Twin Peaks<br>
`Storm::back_poisson(long long) -> long long`: Right Hill<br>
`Storm::quantum_poisson(long long) -> long long`: Poisson Wave, 3 way Monty<br>

- #### Quantum Monty
`Storm::quantum_monty(long long) -> long long`: 3 by 3 way Monty<br>

### ZeroCool Python Examples

#### Produce a front linear distribution of the last five values of a list
```python
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
```python
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
```python
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
`Storm::canonical_variate() -> double`<br>
Flat uniform distribution of the range `(0.0, 1.0)`.

`Storm::random_float(double a, double b) -> double`<br>
Flat uniform distribution of the range `[a, b)`.

`Storm::exponential_variate(double lambda_rate) -> double`<br>
Exponential distribution based on lambda rate.

`Storm::gamma_variate(double shape, double scale) -> double`<br>
Gamma distribution based on shape and scale.

`Storm::weibull_variate(double shape, double scale) -> double`<br>
Weibull distribution based on shape and scale.

`Storm::normal_variate(double mean, double std_dev) -> double`<br>
Normal or Gaussian distribution based on mean and standard deviation.

`Storm::lognormal_variate(double log_mean, double log_deviation) -> double`<br>
Log Normal distribution based on log mean and log standard deviation.

`Storm::extreme_value_variate(double location, double scale) -> double`<br>
Extreme value distribution based on location and scale.

`Storm::chi_squared_variate(double degrees_of_freedom) -> double`<br>
Chi squared distribution based on degrees of freedom.

`Storm::cauchy_variate(double location, double scale) -> double`<br>
Cauchy distribution based on location and scale.

`Storm::fisher_f_variate(double degrees_of_freedom_1, double degrees_of_freedom_2) -> double`<br>
Fisher F distribution based on degrees of freedom across two dimensions.

`Storm::student_t_variate(double degrees_of_freedom) -> double`<br>
Student T distribution based on degrees of freedom.

`Storm::beta_variate(double alpha, double beta) -> double`<br>
Beta distribution based on two inputs alpha and beta.

`Storm::pareto_variate(double alpha) -> double`<br>
Pareto distribution based on input alpha.

`Storm::vonmises_variate(double mu, double kappa) -> double`<br>
Von Mises distribution based on two inputs: mu and kappa.

`Storm::triangular_variate(double low, double high, double mode) -> double`<br>
Triangular distribution based on low, high and mode.


## Performance and Distribution Tests via MonkeyTimer
Testbed Info
- Hardware: Intel Core i9-9880H, 16GB RAM, 1TB SSD
- Software: macOS Catalina 10.15.6, Terminal, MonkeyTimer, Storm
- Compiler  `$ clang++ main.cpp -std=c++17 -Ofast -march=native -o stormtests.o`
- Run Tests: `$ ./stormtests.o`

```
MonkeyTimer
Storm 3.4.2
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

percent_true(25.0): 21 nano
0: 75.129%
1: 24.871%

bernoulli_variate(0.25): 18 nano
0: 75.05%
1: 24.95%


Random Integer 
--------------

random_below(10): 36 nano
0: 9.98%
1: 9.889%
2: 10.111%
3: 10.007%
4: 9.995%
5: 10.067%
6: 9.999%
7: 9.978%
8: 9.791%
9: 10.183%

uniform_int_variate(0, 9): 31 nano
0: 10.107%
1: 9.985%
2: 10.01%
3: 10%
4: 9.905%
5: 10.024%
6: 9.987%
7: 10.044%
8: 9.902%
9: 10.036%

random_range(0, 20, 2): 49 nano
0: 10.023%
2: 9.892%
4: 9.939%
6: 9.936%
8: 10.226%
10: 9.905%
12: 10.019%
14: 10.044%
16: 9.989%
18: 10.027%

random_range(0, 20, -2): 48 nano
2: 9.843%
4: 10.113%
6: 9.827%
8: 10.045%
10: 10.136%
12: 10.05%
14: 10.057%
16: 9.96%
18: 9.957%
20: 10.012%

random_range(0, -20, 2): 48 nano
-20: 10.005%
-18: 10.128%
-16: 10.089%
-14: 10.019%
-12: 9.994%
-10: 9.885%
-8: 10.038%
-6: 9.753%
-4: 9.997%
-2: 10.092%

random_range(0, -20, -2): 46 nano
-18: 9.904%
-16: 10.107%
-14: 10.066%
-12: 9.983%
-10: 9.932%
-8: 10.045%
-6: 10.063%
-4: 10.045%
-2: 9.933%
0: 9.922%

plus_or_minus(10): 30 nano
-10: 4.803%
-9: 4.887%
-8: 4.791%
-7: 4.738%
-6: 4.669%
-5: 4.826%
-4: 4.707%
-3: 4.66%
-2: 4.833%
-1: 4.748%
0: 4.766%
1: 4.759%
2: 4.766%
3: 4.678%
4: 4.809%
5: 4.829%
6: 4.748%
7: 4.704%
8: 4.702%
9: 4.738%
10: 4.839%

plus_or_minus_linear(10): 56 nano
-10: 0.824%
-9: 1.685%
-8: 2.418%
-7: 3.331%
-6: 4.106%
-5: 4.937%
-4: 5.903%
-3: 6.489%
-2: 7.347%
-1: 8.448%
0: 9.052%
1: 8.246%
2: 7.405%
3: 6.668%
4: 5.818%
5: 4.927%
6: 4.182%
7: 3.258%
8: 2.464%
9: 1.699%
10: 0.793%

plus_or_minus_gauss(10): 59 nano
-10: 0.082%
-9: 0.217%
-8: 0.539%
-7: 1.164%
-6: 2.062%
-5: 3.757%
-4: 5.709%
-3: 7.997%
-2: 10.248%
-1: 11.929%
0: 12.533%
1: 11.848%
2: 10.29%
3: 7.906%
4: 5.695%
5: 3.883%
6: 2.135%
7: 1.107%
8: 0.575%
9: 0.226%
10: 0.098%

binomial_variate(10, 0.5): 85 nano
0: 0.108%
1: 0.968%
2: 4.402%
3: 11.746%
4: 20.354%
5: 24.905%
6: 20.377%
7: 11.766%
8: 4.263%
9: 0.996%
10: 0.115%

negative_binomial_variate(5, 0.75): 73 nano
0: 23.685%
1: 29.619%
2: 22.373%
3: 13.052%
4: 6.481%
5: 2.873%
6: 1.185%
7: 0.462%
8: 0.18%
9: 0.054%
10: 0.024%
11: 0.009%
12: 0.001%
13: 0.002%

geometric_variate(0.75): 21 nano
0: 75.038%
1: 18.735%
2: 4.624%
3: 1.21%
4: 0.284%
5: 0.091%
6: 0.013%
7: 0.003%
8: 0.001%
9: 0.001%

poisson_variate(4.0): 77 nano
0: 1.765%
1: 7.428%
2: 14.656%
3: 19.407%
4: 19.331%
5: 15.664%
6: 10.509%
7: 6.058%
8: 2.971%
9: 1.368%
10: 0.562%
11: 0.177%
12: 0.067%
13: 0.028%
14: 0.009%


Random Dice 
-----------

d(10): 29 nano
1: 9.989%
2: 9.936%
3: 9.887%
4: 9.931%
5: 10.059%
6: 9.987%
7: 10.098%
8: 10.047%
9: 10.025%
10: 10.041%

dice(3, 6): 79 nano
3: 0.464%
4: 1.356%
5: 2.698%
6: 4.742%
7: 6.993%
8: 9.695%
9: 11.516%
10: 12.634%
11: 12.343%
12: 11.558%
13: 9.708%
14: 7.025%
15: 4.657%
16: 2.759%
17: 1.405%
18: 0.447%

ability_dice(4): 168 nano
3: 0.056%
4: 0.314%
5: 0.811%
6: 1.653%
7: 2.875%
8: 4.779%
9: 7.21%
10: 9.255%
11: 11.512%
12: 12.884%
13: 13.234%
14: 12.331%
15: 9.941%
16: 7.26%
17: 4.292%
18: 1.593%


Random Index: ZeroCool 
----------------------

F(N) where N = 10

random_index(N): 33 nano
0: 10.218%
1: 10.011%
2: 10.123%
3: 10.01%
4: 9.961%
5: 9.856%
6: 9.868%
7: 9.995%
8: 9.889%
9: 10.069%

front_linear(N): 19 nano
0: 19.233%
1: 16.924%
2: 15.145%
3: 12.811%
4: 10.947%
5: 8.831%
6: 7.101%
7: 5.074%
8: 2.964%
9: 0.97%

middle_linear(N): 21 nano
0: 1.926%
1: 5.869%
2: 9.99%
3: 13.867%
4: 18.18%
5: 18.112%
6: 14.031%
7: 10.049%
8: 5.913%
9: 2.063%

back_linear(N): 15 nano
0: 1.04%
1: 3.048%
2: 4.887%
3: 6.989%
4: 9.041%
5: 11.101%
6: 12.806%
7: 14.964%
8: 16.987%
9: 19.137%

quantum_linear(N): 39 nano
0: 7.33%
1: 8.832%
2: 9.905%
3: 11.188%
4: 12.681%
5: 12.584%
6: 11.401%
7: 10.039%
8: 8.65%
9: 7.39%

front_gauss(N): 23 nano
0: 63.146%
1: 23.422%
2: 8.531%
3: 3.113%
4: 1.093%
5: 0.444%
6: 0.168%
7: 0.053%
8: 0.021%
9: 0.009%

middle_gauss(N): 55 nano
0: 0.004%
1: 0.122%
2: 2.088%
3: 13.476%
4: 34.109%
5: 34.272%
6: 13.719%
7: 2.085%
8: 0.121%
9: 0.004%

back_gauss(N): 25 nano
0: 0.01%
1: 0.02%
2: 0.076%
3: 0.136%
4: 0.444%
5: 1.224%
6: 3.13%
7: 8.614%
8: 23.249%
9: 63.097%

quantum_gauss(N): 57 nano
0: 21.201%
1: 7.77%
2: 3.679%
3: 5.488%
4: 11.808%
5: 11.957%
6: 5.644%
7: 3.607%
8: 7.856%
9: 20.99%

front_poisson(N): 65 nano
0: 8.172%
1: 20.613%
2: 25.715%
3: 21.294%
4: 13.393%
5: 6.575%
6: 2.832%
7: 1%
8: 0.311%
9: 0.095%

middle_poisson(N): 86 nano
0: 4.189%
1: 10.253%
2: 13.405%
3: 12.043%
4: 9.954%
5: 9.863%
6: 12.218%
7: 13.369%
8: 10.547%
9: 4.159%

back_poisson(N): 66 nano
0: 0.087%
1: 0.304%
2: 0.977%
3: 2.835%
4: 6.685%
5: 13.371%
6: 21.225%
7: 25.707%
8: 20.554%
9: 8.255%

quantum_poisson(N): 94 nano
0: 4.18%
1: 10.565%
2: 13.374%
3: 12.016%
4: 10.029%
5: 9.965%
6: 12.056%
7: 13.142%
8: 10.475%
9: 4.198%

quantum_monty(N): 82 nano
0: 10.826%
1: 9.031%
2: 8.893%
3: 9.696%
4: 11.399%
5: 11.659%
6: 9.634%
7: 8.944%
8: 9.076%
9: 10.842%

F(-N) where N = 10

random_index(-N): 39 nano
-10: 9.989%
-9: 10.139%
-8: 10.087%
-7: 10.316%
-6: 10.027%
-5: 9.778%
-4: 9.824%
-3: 9.946%
-2: 9.892%
-1: 10.002%

front_linear(-N): 17 nano
-10: 19.021%
-9: 16.905%
-8: 15.036%
-7: 12.929%
-6: 11.025%
-5: 9.026%
-4: 6.954%
-3: 4.995%
-2: 3.031%
-1: 1.078%

middle_linear(-N): 23 nano
-10: 2.011%
-9: 5.92%
-8: 9.871%
-7: 14.152%
-6: 17.744%
-5: 18.062%
-4: 14.161%
-3: 9.957%
-2: 6.063%
-1: 2.059%

back_linear(-N): 16 nano
-10: 1.033%
-9: 2.966%
-8: 5.07%
-7: 6.848%
-6: 8.921%
-5: 11.115%
-4: 13.09%
-3: 14.85%
-2: 17.092%
-1: 19.015%

quantum_linear(-N): 41 nano
-10: 7.442%
-9: 8.706%
-8: 10.047%
-7: 11.425%
-6: 12.585%
-5: 12.771%
-4: 11.522%
-3: 9.862%
-2: 8.521%
-1: 7.119%

front_gauss(-N): 24 nano
-10: 63.131%
-9: 23.323%
-8: 8.525%
-7: 3.193%
-6: 1.142%
-5: 0.421%
-4: 0.172%
-3: 0.059%
-2: 0.022%
-1: 0.012%

middle_gauss(-N): 51 nano
-10: 0.001%
-9: 0.148%
-8: 2.101%
-7: 13.581%
-6: 34.395%
-5: 33.824%
-4: 13.646%
-3: 2.174%
-2: 0.128%
-1: 0.002%

back_gauss(-N): 24 nano
-10: 0.016%
-9: 0.016%
-8: 0.058%
-7: 0.149%
-6: 0.477%
-5: 1.124%
-4: 3.152%
-3: 8.564%
-2: 23.297%
-1: 63.147%

quantum_gauss(-N): 55 nano
-10: 21.023%
-9: 7.769%
-8: 3.592%
-7: 5.667%
-6: 12.068%
-5: 11.801%
-4: 5.597%
-3: 3.542%
-2: 7.88%
-1: 21.061%

front_poisson(-N): 64 nano
-10: 8.108%
-9: 20.522%
-8: 25.653%
-7: 21.768%
-6: 13.182%
-5: 6.656%
-4: 2.697%
-3: 1.006%
-2: 0.323%
-1: 0.085%

middle_poisson(-N): 84 nano
-10: 4.121%
-9: 10.285%
-8: 13.366%
-7: 11.986%
-6: 9.904%
-5: 10.199%
-4: 12.191%
-3: 13.342%
-2: 10.525%
-1: 4.081%

back_poisson(-N): 68 nano
-10: 0.086%
-9: 0.321%
-8: 0.994%
-7: 2.832%
-6: 6.742%
-5: 13.359%
-4: 21.293%
-3: 25.672%
-2: 20.462%
-1: 8.239%

quantum_poisson(-N): 95 nano
-10: 4.148%
-9: 10.485%
-8: 13.23%
-7: 12.147%
-6: 9.984%
-5: 9.954%
-4: 11.891%
-3: 13.415%
-2: 10.549%
-1: 4.197%

quantum_monty(-N): 82 nano
-10: 10.864%
-9: 9.079%
-8: 8.844%
-7: 9.739%
-6: 11.569%
-5: 11.522%
-4: 9.771%
-3: 8.897%
-2: 9.05%
-1: 10.665%


Random Float: timer only 
------------------------

canonical_variate(): 13 nano
uniform_real_variate(1.0, 10.0): 12 nano
exponential_variate(1.0): 13 nano
gamma_variate(1.0, 1.0): 21 nano
weibull_variate(1.0, 1.0): 12 nano
normal_variate(5, 2): 34 nano
lognormal_variate(1.6, 0.25): 31 nano
extreme_value_variate(0.0, 1.0): 12 nano
chi_squared_variate(1.0): 65 nano
cauchy_variate(0.0, 0.00005): 12 nano
fisher_f_variate(1.0, 1.0): 145 nano
student_t_variate(1.0): 136 nano
beta_variate(10.0, 1.0): 84 nano
pareto_variate(5.0): 12 nano
vonmises_variate(1.0, 1.0): 90 nano
triangular_variate(0.0, 10.0, 5.0): 12 nano


Clamp Tests 
-----------

clamp(50, 1, 100): 0 nano
smart_clamp(50, 1, 100): 0 nano


============================
Total Time: 0.594155 seconds

```
