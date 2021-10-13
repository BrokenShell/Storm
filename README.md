#  Storm v3.5.1
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
`Storm::GetFloat::canonical_variate() -> long double`<br>
Flat uniform distribution of the range `(0.0, 1.0)`.

`Storm::GetFloat::uniform_real_variate(long double a, long double b) -> long double`<br>
Flat uniform distribution of the range `[a, b)`.

`Storm::GetFloat::exponential_variate(long double lambda_rate) -> long double`<br>
Exponential distribution based on lambda rate.

`Storm::GetFloat::gamma_variate(long double shape, long double scale) -> long double`<br>
Gamma distribution based on shape and scale.

`Storm::GetFloat::weibull_variate(long double shape, long double scale) -> long double`<br>
Weibull distribution based on shape and scale.

`Storm::GetFloat::normal_variate(long double mean, long double std_dev) -> long double`<br>
Normal or Gaussian distribution based on mean and standard deviation.

`Storm::GetFloat::lognormal_variate(long double log_mean, long double log_deviation) -> long double`<br>
Log Normal distribution based on log mean and log standard deviation.

`Storm::GetFloat::extreme_value_variate(long double location, long double scale) -> long double`<br>
Extreme value distribution based on location and scale.

`Storm::GetFloat::chi_squared_variate(double degrees_of_freedom) -> long double`<br>
Chi squared distribution based on degrees of freedom.

`Storm::GetFloat::cauchy_variate(long double location, long double scale) -> long double`<br>
Cauchy distribution based on location and scale.

`Storm::GetFloat::fisher_f_variate(double degrees_of_freedom_1, double degrees_of_freedom_2) -> long double`<br>
Fisher F distribution based on degrees of freedom across two dimensions.

`Storm::GetFloat::student_t_variate(double degrees_of_freedom) -> long double`<br>
Student T distribution based on degrees of freedom.

`Storm::GetFloat::beta_variate(long double alpha, long double beta) -> long double`<br>
Beta distribution based on two inputs alpha and beta.

`Storm::GetFloat::pareto_variate(long double alpha) -> long double`<br>
Pareto distribution based on input alpha.

`Storm::GetFloat::vonmises_variate(long double mu, long double kappa) -> long double`<br>
Von Mises distribution based on two inputs: mu and kappa.

`Storm::GetFloat::triangular_variate(long double low, long double high, long double mode) -> long double`<br>
Triangular distribution based on low, high and mode.


## Performance and Distribution Tests via MonkeyTimer
Testbed Info
- Hardware: Intel Core i9-9880H, 16GB RAM, 1TB SSD
- Software: macOS Big Sur 11.5.2, MonkeyTimer, Storm
- Compiler  `$ clang++ main.cpp -std=c++17 -Ofast -march=native -o stormtests.o`
- Run Tests: `$ ./stormtests.o`

```
MonkeyTimer
Storm Version 3.5.1
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
percent_true(25.0): 19 nano
0: 74.88%
1: 25.12%

bernoulli_variate(0.25): 22 nano
0: 74.918%
1: 25.082%

Random Integer 
--------------
random_below(10): 47 nano
0: 9.976%
1: 10.125%
2: 9.915%
3: 9.924%
4: 10.038%
5: 9.972%
6: 9.881%
7: 9.978%
8: 9.959%
9: 10.232%

uniform_int_variate(0, 9): 44 nano
0: 9.828%
1: 10.094%
2: 9.945%
3: 9.957%
4: 10.035%
5: 10.07%
6: 10.236%
7: 9.812%
8: 10.006%
9: 10.017%

random_range(0, 20, 2): 65 nano
0: 10.015%
2: 10.019%
4: 9.93%
6: 9.936%
8: 10.04%
10: 9.986%
12: 9.92%
14: 10.087%
16: 9.923%
18: 10.144%

random_range(0, 20, -2): 62 nano
2: 9.924%
4: 9.833%
6: 10.049%
8: 10.099%
10: 10.08%
12: 10.081%
14: 9.83%
16: 10.246%
18: 9.932%
20: 9.926%

random_range(0, -20, 2): 55 nano
-20: 9.994%
-18: 9.966%
-16: 9.866%
-14: 9.898%
-12: 9.993%
-10: 9.965%
-8: 10.238%
-6: 9.909%
-4: 10.109%
-2: 10.062%

random_range(0, -20, -2): 55 nano
-18: 9.946%
-16: 9.93%
-14: 10.028%
-12: 9.839%
-10: 10.199%
-8: 10.018%
-6: 10.052%
-4: 9.95%
-2: 10.034%
0: 10.004%

plus_or_minus(10): 33 nano
-10: 4.659%
-9: 4.957%
-8: 4.725%
-7: 4.764%
-6: 4.714%
-5: 4.649%
-4: 4.625%
-3: 4.792%
-2: 4.887%
-1: 4.781%
0: 4.872%
1: 4.729%
2: 4.673%
3: 4.783%
4: 4.852%
5: 4.722%
6: 4.812%
7: 4.743%
8: 4.707%
9: 4.75%
10: 4.804%

plus_or_minus_linear(10): 72 nano
-10: 0.862%
-9: 1.64%
-8: 2.489%
-7: 3.239%
-6: 4.108%
-5: 5.054%
-4: 5.809%
-3: 6.655%
-2: 7.268%
-1: 8.282%
0: 9.224%
1: 8.177%
2: 7.444%
3: 6.494%
4: 5.804%
5: 4.938%
6: 4.157%
7: 3.376%
8: 2.467%
9: 1.674%
10: 0.839%

plus_or_minus_gauss(10): 65 nano
-10: 0.113%
-9: 0.227%
-8: 0.531%
-7: 1.119%
-6: 2.119%
-5: 3.738%
-4: 5.699%
-3: 8.128%
-2: 10.302%
-1: 11.863%
0: 12.315%
1: 11.859%
2: 10.288%
3: 8.138%
4: 5.708%
5: 3.622%
6: 2.205%
7: 1.115%
8: 0.569%
9: 0.222%
10: 0.12%

binomial_variate(10, 0.5): 91 nano
0: 0.112%
1: 0.993%
2: 4.43%
3: 11.624%
4: 20.476%
5: 24.37%
6: 20.713%
7: 11.809%
8: 4.386%
9: 0.978%
10: 0.109%

negative_binomial_variate(5, 0.75): 76 nano
0: 23.641%
1: 29.568%
2: 22.213%
3: 13.073%
4: 6.426%
5: 3.039%
6: 1.248%
7: 0.517%
8: 0.17%
9: 0.073%
10: 0.02%
11: 0.008%
12: 0.003%
13: 0.001%

geometric_variate(0.75): 21 nano
0: 74.952%
1: 18.773%
2: 4.701%
3: 1.151%
4: 0.314%
5: 0.084%
6: 0.017%
7: 0.005%
8: 0.002%
9: 0.001%

poisson_variate(4.0): 70 nano
0: 1.786%
1: 7.325%
2: 14.712%
3: 19.676%
4: 19.427%
5: 15.613%
6: 10.403%
7: 5.934%
8: 2.919%
9: 1.376%
10: 0.542%
11: 0.199%
12: 0.069%
13: 0.014%
14: 0.004%
16: 0.001%

Random Dice 
-----------
d(10): 31 nano
1: 9.995%
2: 10.07%
3: 10.143%
4: 9.946%
5: 10.008%
6: 9.898%
7: 9.871%
8: 10.029%
9: 10.088%
10: 9.952%

dice(3, 6): 79 nano
3: 0.449%
4: 1.327%
5: 2.747%
6: 4.632%
7: 7.2%
8: 9.597%
9: 11.571%
10: 12.618%
11: 12.497%
12: 11.716%
13: 9.538%
14: 6.951%
15: 4.576%
16: 2.771%
17: 1.339%
18: 0.471%

dice(0, 6): 2 nano
0: 100%

ability_dice(4): 159 nano
3: 0.08%
4: 0.325%
5: 0.765%
6: 1.653%
7: 2.857%
8: 4.768%
9: 7.062%
10: 9.4%
11: 11.411%
12: 12.979%
13: 13.286%
14: 12.301%
15: 10.18%
16: 7.137%
17: 4.154%
18: 1.642%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 32 nano
0: 10.068%
1: 10.156%
2: 10.149%
3: 9.871%
4: 9.908%
5: 9.95%
6: 9.937%
7: 9.895%
8: 9.983%
9: 10.083%

front_linear(N): 15 nano
0: 19.027%
1: 17.064%
2: 14.971%
3: 13.044%
4: 11.003%
5: 8.85%
6: 7.085%
7: 5.023%
8: 2.924%
9: 1.009%

middle_linear(N): 20 nano
0: 1.995%
1: 5.997%
2: 9.989%
3: 14.046%
4: 18.075%
5: 18.096%
6: 13.689%
7: 10.092%
8: 6.061%
9: 1.96%

back_linear(N): 16 nano
0: 0.998%
1: 2.995%
2: 5.018%
3: 7.138%
4: 8.845%
5: 10.927%
6: 13.076%
7: 14.959%
8: 17.051%
9: 18.993%

quantum_linear(N): 39 nano
0: 7.316%
1: 8.598%
2: 9.973%
3: 11.329%
4: 12.681%
5: 12.62%
6: 11.444%
7: 10.048%
8: 8.621%
9: 7.37%

front_gauss(N): 23 nano
0: 63.486%
1: 23.164%
2: 8.367%
3: 3.116%
4: 1.151%
5: 0.474%
6: 0.146%
7: 0.069%
8: 0.025%
9: 0.002%

middle_gauss(N): 57 nano
0: 0.001%
1: 0.133%
2: 2.097%
3: 13.629%
4: 34.034%
5: 34.225%
6: 13.616%
7: 2.107%
8: 0.156%
9: 0.002%

back_gauss(N): 25 nano
0: 0.008%
1: 0.026%
2: 0.059%
3: 0.163%
4: 0.423%
5: 1.172%
6: 3.081%
7: 8.54%
8: 23.432%
9: 63.096%

quantum_gauss(N): 58 nano
0: 21.25%
1: 7.641%
2: 3.569%
3: 5.64%
4: 11.759%
5: 12.129%
6: 5.657%
7: 3.528%
8: 7.884%
9: 20.943%

front_poisson(N): 57 nano
0: 8.262%
1: 20.474%
2: 25.774%
3: 21.332%
4: 13.417%
5: 6.522%
6: 2.772%
7: 1.051%
8: 0.304%
9: 0.092%

middle_poisson(N): 79 nano
0: 4.087%
1: 10.462%
2: 13.139%
3: 12.089%
4: 9.911%
5: 10.123%
6: 12.174%
7: 13.398%
8: 10.409%
9: 4.208%

back_poisson(N): 57 nano
0: 0.064%
1: 0.284%
2: 1.011%
3: 2.875%
4: 6.798%
5: 13.264%
6: 21.182%
7: 25.705%
8: 20.629%
9: 8.188%

quantum_poisson(N): 88 nano
0: 4.071%
1: 10.585%
2: 13.29%
3: 12.283%
4: 9.997%
5: 10.047%
6: 11.933%
7: 13.135%
8: 10.465%
9: 4.194%

quantum_monty(N): 85 nano
0: 10.667%
1: 8.895%
2: 8.809%
3: 9.762%
4: 11.663%
5: 11.647%
6: 9.721%
7: 8.901%
8: 9.17%
9: 10.765%

F(-N) where N = 10
random_index(-N): 34 nano
-10: 10.096%
-9: 9.988%
-8: 10.032%
-7: 10.015%
-6: 9.755%
-5: 10.132%
-4: 9.958%
-3: 9.961%
-2: 9.951%
-1: 10.112%

front_linear(-N): 15 nano
-10: 18.936%
-9: 16.846%
-8: 15.19%
-7: 13.031%
-6: 11.026%
-5: 8.936%
-4: 6.982%
-3: 4.985%
-2: 3.074%
-1: 0.994%

middle_linear(-N): 20 nano
-10: 2.004%
-9: 6.042%
-8: 10.252%
-7: 14.064%
-6: 17.813%
-5: 17.751%
-4: 13.938%
-3: 10.025%
-2: 6.13%
-1: 1.981%

back_linear(-N): 16 nano
-10: 0.964%
-9: 2.957%
-8: 4.928%
-7: 6.942%
-6: 9.003%
-5: 11.011%
-4: 13.061%
-3: 15.116%
-2: 17.136%
-1: 18.882%

quantum_linear(-N): 39 nano
-10: 7.222%
-9: 8.588%
-8: 9.989%
-7: 11.334%
-6: 12.542%
-5: 12.784%
-4: 11.251%
-3: 10.125%
-2: 8.759%
-1: 7.406%

front_gauss(-N): 24 nano
-10: 63.307%
-9: 23.1%
-8: 8.602%
-7: 3.177%
-6: 1.201%
-5: 0.384%
-4: 0.146%
-3: 0.049%
-2: 0.025%
-1: 0.009%

middle_gauss(-N): 52 nano
-10: 0.002%
-9: 0.133%
-8: 2.175%
-7: 13.59%
-6: 34.007%
-5: 34.104%
-4: 13.74%
-3: 2.115%
-2: 0.13%
-1: 0.004%

back_gauss(-N): 23 nano
-10: 0.004%
-9: 0.024%
-8: 0.074%
-7: 0.17%
-6: 0.426%
-5: 1.17%
-4: 3.171%
-3: 8.605%
-2: 23.072%
-1: 63.284%

quantum_gauss(-N): 55 nano
-10: 21.083%
-9: 7.867%
-8: 3.512%
-7: 5.754%
-6: 11.743%
-5: 11.82%
-4: 5.564%
-3: 3.633%
-2: 7.898%
-1: 21.126%

front_poisson(-N): 63 nano
-10: 8.222%
-9: 20.369%
-8: 25.991%
-7: 21.286%
-6: 13.196%
-5: 6.835%
-4: 2.737%
-3: 0.958%
-2: 0.323%
-1: 0.083%

middle_poisson(-N): 86 nano
-10: 4.2%
-9: 10.346%
-8: 13.346%
-7: 12.166%
-6: 9.957%
-5: 10.052%
-4: 11.968%
-3: 13.268%
-2: 10.55%
-1: 4.147%

back_poisson(-N): 65 nano
-10: 0.074%
-9: 0.369%
-8: 0.967%
-7: 2.861%
-6: 6.583%
-5: 13.452%
-4: 21.333%
-3: 25.559%
-2: 20.512%
-1: 8.29%

quantum_poisson(-N): 90 nano
-10: 4.246%
-9: 10.374%
-8: 13.288%
-7: 12.17%
-6: 10.18%
-5: 10.032%
-4: 12.112%
-3: 13.419%
-2: 10.197%
-1: 3.982%

quantum_monty(-N): 93 nano
-10: 10.993%
-9: 8.838%
-8: 8.923%
-7: 9.671%
-6: 11.537%
-5: 11.562%
-4: 9.648%
-3: 9.008%
-2: 8.873%
-1: 10.947%

Random Float: timer only 
------------------------
canonical_variate(): 14 nano
uniform_real_variate(1.0, 10.0): 13 nano
exponential_variate(1.0): 11 nano
gamma_variate(1.0, 1.0): 21 nano
weibull_variate(1.0, 1.0): 11 nano
normal_variate(5, 2): 31 nano
lognormal_variate(1.6, 0.25): 34 nano
extreme_value_variate(0.0, 1.0): 11 nano
chi_squared_variate(1.0): 70 nano
cauchy_variate(0.0, 0.00005): 12 nano
fisher_f_variate(1.0, 1.0): 137 nano
student_t_variate(1.0): 117 nano
beta_variate(10.0, 1.0): 84 nano
pareto_variate(5.0): 12 nano
vonmises_variate(1.0, 1.0): 92 nano
triangular_variate(0.0, 10.0, 5.0): 11 nano

Clamp Tests 
-----------
clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano

============================
Total Time: 0.606731 seconds

```
