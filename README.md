#  Storm v3.4.3
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
- Software: macOS Big Sur 11.2, Terminal, MonkeyTimer, Storm
- Compiler  `$ clang++ main.cpp -std=c++17 -Ofast -march=native -o stormtests.o`
- Run Tests: `$ ./stormtests.o`

```
MonkeyTimer
Storm 3.4.3
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

percent_true(25.0): 14 nano
0: 75.045%
1: 24.955%

bernoulli_variate(0.25): 15 nano
0: 75.057%
1: 24.943%


Random Integer 
--------------

random_below(10): 36 nano
0: 9.997%
1: 9.979%
2: 10%
3: 9.911%
4: 10.082%
5: 10.13%
6: 10.123%
7: 9.998%
8: 9.863%
9: 9.917%

uniform_int_variate(0, 9): 31 nano
0: 10.071%
1: 9.914%
2: 10.042%
3: 10.184%
4: 10.035%
5: 9.813%
6: 9.97%
7: 9.977%
8: 10.035%
9: 9.959%

random_range(0, 20, 2): 50 nano
0: 9.927%
2: 10.142%
4: 10.141%
6: 9.993%
8: 10.027%
10: 9.835%
12: 9.938%
14: 9.985%
16: 10.011%
18: 10.001%

random_range(0, 20, -2): 45 nano
2: 10.106%
4: 9.939%
6: 10.177%
8: 10.084%
10: 10.149%
12: 10.035%
14: 9.845%
16: 9.714%
18: 9.878%
20: 10.073%

random_range(0, -20, 2): 48 nano
-20: 10.06%
-18: 10.039%
-16: 9.961%
-14: 10.045%
-12: 10.086%
-10: 9.934%
-8: 10.011%
-6: 9.907%
-4: 10.034%
-2: 9.923%

random_range(0, -20, -2): 44 nano
-18: 10.029%
-16: 9.829%
-14: 10.048%
-12: 9.912%
-10: 10.076%
-8: 9.938%
-6: 9.879%
-4: 10.126%
-2: 10.039%
0: 10.124%

plus_or_minus(10): 30 nano
-10: 4.931%
-9: 4.798%
-8: 4.823%
-7: 4.809%
-6: 4.624%
-5: 4.652%
-4: 4.871%
-3: 4.772%
-2: 4.794%
-1: 4.789%
0: 4.648%
1: 4.735%
2: 4.763%
3: 4.646%
4: 4.77%
5: 4.778%
6: 4.825%
7: 4.769%
8: 4.77%
9: 4.74%
10: 4.693%

plus_or_minus_linear(10): 56 nano
-10: 0.831%
-9: 1.626%
-8: 2.434%
-7: 3.422%
-6: 4.166%
-5: 5.026%
-4: 5.781%
-3: 6.754%
-2: 7.38%
-1: 8.29%
0: 8.988%
1: 8.248%
2: 7.299%
3: 6.7%
4: 5.857%
5: 4.904%
6: 4.18%
7: 3.224%
8: 2.432%
9: 1.652%
10: 0.806%

plus_or_minus_gauss(10): 60 nano
-10: 0.107%
-9: 0.243%
-8: 0.552%
-7: 1.16%
-6: 2.174%
-5: 3.727%
-4: 5.73%
-3: 7.969%
-2: 10.243%
-1: 11.765%
0: 12.486%
1: 11.902%
2: 10.244%
3: 7.961%
4: 5.797%
5: 3.716%
6: 2.186%
7: 1.177%
8: 0.513%
9: 0.245%
10: 0.103%

binomial_variate(10, 0.5): 96 nano
0: 0.1%
1: 0.992%
2: 4.372%
3: 11.65%
4: 20.615%
5: 24.672%
6: 20.37%
7: 11.772%
8: 4.388%
9: 0.96%
10: 0.109%

negative_binomial_variate(5, 0.75): 82 nano
0: 23.715%
1: 29.589%
2: 22.499%
3: 12.876%
4: 6.468%
5: 2.889%
6: 1.227%
7: 0.459%
8: 0.171%
9: 0.073%
10: 0.02%
11: 0.009%
12: 0.004%
13: 0.001%

geometric_variate(0.75): 21 nano
0: 75.013%
1: 18.74%
2: 4.636%
3: 1.228%
4: 0.286%
5: 0.069%
6: 0.022%
7: 0.006%

poisson_variate(4.0): 65 nano
0: 1.792%
1: 7.222%
2: 14.543%
3: 19.772%
4: 19.695%
5: 15.601%
6: 10.342%
7: 5.929%
8: 2.966%
9: 1.304%
10: 0.536%
11: 0.211%
12: 0.067%
13: 0.012%
14: 0.007%
16: 0.001%


Random Dice 
-----------

d(10): 29 nano
1: 9.942%
2: 10.157%
3: 9.938%
4: 9.947%
5: 10.048%
6: 9.895%
7: 10.068%
8: 10.004%
9: 9.945%
10: 10.056%

dice(3, 6): 81 nano
3: 0.469%
4: 1.394%
5: 2.833%
6: 4.698%
7: 7.004%
8: 9.609%
9: 11.668%
10: 12.439%
11: 12.488%
12: 11.557%
13: 9.733%
14: 6.861%
15: 4.586%
16: 2.784%
17: 1.397%
18: 0.48%

ability_dice(4): 153 nano
3: 0.091%
4: 0.279%
5: 0.763%
6: 1.596%
7: 3.02%
8: 4.714%
9: 7.26%
10: 9.301%
11: 11.361%
12: 12.956%
13: 13.141%
14: 12.406%
15: 10.146%
16: 7.171%
17: 4.134%
18: 1.661%


Random Index: ZeroCool 
----------------------

F(N) where N = 10

random_index(N): 33 nano
0: 10.032%
1: 10.058%
2: 9.947%
3: 10.143%
4: 10.079%
5: 9.82%
6: 9.971%
7: 9.924%
8: 9.959%
9: 10.067%

front_linear(N): 19 nano
0: 19.047%
1: 16.812%
2: 15.031%
3: 12.905%
4: 11.187%
5: 8.951%
6: 7.089%
7: 5.084%
8: 2.916%
9: 0.978%

middle_linear(N): 26 nano
0: 2.033%
1: 5.827%
2: 10.098%
3: 14.016%
4: 18.187%
5: 17.74%
6: 13.964%
7: 10.049%
8: 6.027%
9: 2.059%

back_linear(N): 16 nano
0: 1.019%
1: 2.976%
2: 4.933%
3: 7.167%
4: 8.906%
5: 11.049%
6: 12.716%
7: 15.19%
8: 16.948%
9: 19.096%

quantum_linear(N): 40 nano
0: 7.311%
1: 8.71%
2: 9.973%
3: 11.576%
4: 12.742%
5: 12.653%
6: 11.333%
7: 9.892%
8: 8.526%
9: 7.284%

front_gauss(N): 25 nano
0: 63.29%
1: 23.162%
2: 8.511%
3: 3.146%
4: 1.194%
5: 0.435%
6: 0.164%
7: 0.063%
8: 0.027%
9: 0.008%

middle_gauss(N): 56 nano
0: 0.001%
1: 0.127%
2: 2.18%
3: 13.602%
4: 34.052%
5: 34.098%
6: 13.639%
7: 2.16%
8: 0.14%
9: 0.001%

back_gauss(N): 24 nano
0: 0.006%
1: 0.021%
2: 0.062%
3: 0.148%
4: 0.441%
5: 1.105%
6: 3.191%
7: 8.628%
8: 23.461%
9: 62.937%

quantum_gauss(N): 59 nano
0: 20.95%
1: 7.767%
2: 3.577%
3: 5.738%
4: 11.905%
5: 11.938%
6: 5.602%
7: 3.574%
8: 7.785%
9: 21.164%

front_poisson(N): 60 nano
0: 8.304%
1: 20.538%
2: 25.642%
3: 21.226%
4: 13.424%
5: 6.661%
6: 2.824%
7: 0.972%
8: 0.317%
9: 0.092%

middle_poisson(N): 89 nano
0: 4.17%
1: 10.402%
2: 13.462%
3: 12.157%
4: 9.954%
5: 9.983%
6: 12.027%
7: 13.283%
8: 10.4%
9: 4.162%

back_poisson(N): 60 nano
0: 0.08%
1: 0.286%
2: 0.944%
3: 2.802%
4: 6.69%
5: 13.226%
6: 21.451%
7: 25.806%
8: 20.53%
9: 8.185%

quantum_poisson(N): 92 nano
0: 4.136%
1: 10.371%
2: 13.318%
3: 12.372%
4: 10.015%
5: 9.962%
6: 12.134%
7: 13.116%
8: 10.36%
9: 4.216%

quantum_monty(N): 96 nano
0: 10.628%
1: 8.851%
2: 8.969%
3: 9.722%
4: 11.464%
5: 11.557%
6: 10.035%
7: 8.918%
8: 8.948%
9: 10.908%

F(-N) where N = 10

random_index(-N): 37 nano
-10: 10.041%
-9: 10.166%
-8: 9.85%
-7: 9.999%
-6: 9.832%
-5: 10.102%
-4: 10.017%
-3: 10.099%
-2: 9.871%
-1: 10.023%

front_linear(-N): 18 nano
-10: 18.993%
-9: 16.937%
-8: 15.133%
-7: 13.065%
-6: 10.947%
-5: 8.962%
-4: 6.982%
-3: 4.944%
-2: 2.993%
-1: 1.044%

middle_linear(-N): 25 nano
-10: 1.934%
-9: 5.962%
-8: 10.01%
-7: 13.877%
-6: 18.061%
-5: 18.149%
-4: 14.026%
-3: 10.048%
-2: 5.952%
-1: 1.981%

back_linear(-N): 20 nano
-10: 0.99%
-9: 3.047%
-8: 4.991%
-7: 6.932%
-6: 8.938%
-5: 10.932%
-4: 13.142%
-3: 15.102%
-2: 16.921%
-1: 19.005%

quantum_linear(-N): 48 nano
-10: 7.345%
-9: 8.565%
-8: 10.183%
-7: 11.429%
-6: 12.743%
-5: 12.472%
-4: 11.271%
-3: 10.014%
-2: 8.774%
-1: 7.204%

front_gauss(-N): 34 nano
-10: 63.416%
-9: 23.153%
-8: 8.439%
-7: 3.168%
-6: 1.124%
-5: 0.435%
-4: 0.156%
-3: 0.075%
-2: 0.027%
-1: 0.007%

middle_gauss(-N): 67 nano
-10: 0.004%
-9: 0.115%
-8: 2.138%
-7: 13.517%
-6: 34.074%
-5: 34.334%
-4: 13.554%
-3: 2.122%
-2: 0.139%
-1: 0.003%

back_gauss(-N): 32 nano
-10: 0.008%
-9: 0.022%
-8: 0.065%
-7: 0.163%
-6: 0.416%
-5: 1.174%
-4: 3.067%
-3: 8.697%
-2: 23.089%
-1: 63.299%

quantum_gauss(-N): 70 nano
-10: 20.969%
-9: 7.825%
-8: 3.581%
-7: 5.516%
-6: 11.907%
-5: 11.994%
-4: 5.609%
-3: 3.617%
-2: 7.82%
-1: 21.162%

front_poisson(-N): 63 nano
-10: 8.257%
-9: 20.514%
-8: 25.672%
-7: 21.401%
-6: 13.498%
-5: 6.542%
-4: 2.769%
-3: 0.962%
-2: 0.285%
-1: 0.1%

middle_poisson(-N): 86 nano
-10: 4.299%
-9: 10.272%
-8: 13.278%
-7: 12.094%
-6: 10.067%
-5: 10.11%
-4: 12.046%
-3: 13.275%
-2: 10.377%
-1: 4.182%

back_poisson(-N): 61 nano
-10: 0.096%
-9: 0.298%
-8: 0.985%
-7: 2.82%
-6: 6.807%
-5: 13.394%
-4: 21.394%
-3: 25.689%
-2: 20.345%
-1: 8.172%

quantum_poisson(-N): 94 nano
-10: 4.174%
-9: 10.492%
-8: 13.331%
-7: 12.19%
-6: 10.075%
-5: 9.891%
-4: 12.13%
-3: 13.208%
-2: 10.346%
-1: 4.163%

quantum_monty(-N): 97 nano
-10: 11.088%
-9: 8.814%
-8: 8.963%
-7: 9.553%
-6: 11.707%
-5: 11.53%
-4: 9.666%
-3: 8.811%
-2: 8.991%
-1: 10.877%


Random Float: timer only 
------------------------

canonical_variate(): 14 nano
uniform_real_variate(1.0, 10.0): 14 nano
exponential_variate(1.0): 14 nano
gamma_variate(1.0, 1.0): 25 nano
weibull_variate(1.0, 1.0): 14 nano
normal_variate(5, 2): 39 nano
lognormal_variate(1.6, 0.25): 38 nano
extreme_value_variate(0.0, 1.0): 14 nano
chi_squared_variate(1.0): 83 nano
cauchy_variate(0.0, 0.00005): 14 nano
fisher_f_variate(1.0, 1.0): 164 nano
student_t_variate(1.0): 158 nano
beta_variate(10.0, 1.0): 101 nano
pareto_variate(5.0): 14 nano
vonmises_variate(1.0, 1.0): 176 nano
triangular_variate(0.0, 10.0, 5.0): 13 nano


Clamp Tests 
-----------

clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano


============================
Total Time: 0.645645 seconds

```
