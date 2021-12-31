#  Storm v3.5.4
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
- Software: macOS Big Sur 11.6.2, MonkeyTimer, Storm
- Compile  `$ clang++ main.cpp -std=c++17 -Ofast -march=native -o stormtests.o`
- Run Tests: `$ ./stormtests.o`

```
MonkeyTimer
Storm Version: 3.5.4
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
0: 74.862%
1: 25.138%

bernoulli_variate(0.25): 20 nano
0: 75.115%
1: 24.885%

Random Integer 
--------------
random_below(10): 42 nano
0: 9.909%
1: 9.889%
2: 10.072%
3: 9.892%
4: 9.974%
5: 9.968%
6: 10.188%
7: 10.07%
8: 10.215%
9: 9.823%

uniform_int_variate(0, 9): 38 nano
0: 9.98%
1: 9.959%
2: 10.101%
3: 9.982%
4: 9.975%
5: 9.983%
6: 10.006%
7: 9.959%
8: 10.167%
9: 9.888%

random_range(0, 20, 2): 52 nano
0: 9.865%
2: 9.76%
4: 10.012%
6: 10.009%
8: 10.014%
10: 10.163%
12: 9.996%
14: 9.992%
16: 9.999%
18: 10.19%

random_range(0, 20, -2): 50 nano
2: 9.992%
4: 9.958%
6: 10.031%
8: 10.101%
10: 9.974%
12: 9.801%
14: 9.929%
16: 10.081%
18: 10.089%
20: 10.044%

random_range(0, -20, 2): 42 nano
-20: 9.819%
-18: 9.878%
-16: 10.234%
-14: 9.952%
-12: 9.967%
-10: 10.091%
-8: 10.007%
-6: 10.001%
-4: 10.037%
-2: 10.014%

random_range(0, -20, -2): 42 nano
-18: 9.993%
-16: 10.155%
-14: 10.053%
-12: 10.123%
-10: 10.019%
-8: 9.8%
-6: 10.146%
-4: 9.951%
-2: 9.931%
0: 9.829%

plus_or_minus(10): 27 nano
-10: 4.762%
-9: 4.684%
-8: 4.837%
-7: 4.855%
-6: 4.729%
-5: 4.675%
-4: 4.664%
-3: 4.706%
-2: 4.762%
-1: 4.803%
0: 4.826%
1: 4.831%
2: 4.737%
3: 4.772%
4: 4.759%
5: 4.722%
6: 4.669%
7: 4.782%
8: 4.872%
9: 4.755%
10: 4.798%

plus_or_minus_linear(10): 49 nano
-10: 0.857%
-9: 1.704%
-8: 2.434%
-7: 3.311%
-6: 4.08%
-5: 4.999%
-4: 5.732%
-3: 6.663%
-2: 7.303%
-1: 8.247%
0: 9.266%
1: 8.113%
2: 7.547%
3: 6.535%
4: 5.79%
5: 4.985%
6: 4.156%
7: 3.327%
8: 2.442%
9: 1.665%
10: 0.844%

plus_or_minus_gauss(10): 56 nano
-10: 0.11%
-9: 0.247%
-8: 0.53%
-7: 1.107%
-6: 2.17%
-5: 3.665%
-4: 5.652%
-3: 8.042%
-2: 10.298%
-1: 11.869%
0: 12.495%
1: 11.911%
2: 10.261%
3: 8.062%
4: 5.74%
5: 3.791%
6: 2.128%
7: 1.089%
8: 0.521%
9: 0.231%
10: 0.081%

binomial_variate(10, 0.5): 89 nano
0: 0.081%
1: 0.973%
2: 4.371%
3: 11.895%
4: 20.271%
5: 24.734%
6: 20.463%
7: 11.678%
8: 4.442%
9: 1.001%
10: 0.091%

negative_binomial_variate(5, 0.75): 78 nano
0: 23.545%
1: 29.633%
2: 22.501%
3: 12.931%
4: 6.484%
5: 2.956%
6: 1.199%
7: 0.469%
8: 0.178%
9: 0.063%
10: 0.03%
11: 0.006%
12: 0.003%
13: 0.002%

geometric_variate(0.75): 25 nano
0: 75.057%
1: 18.643%
2: 4.719%
3: 1.202%
4: 0.277%
5: 0.08%
6: 0.016%
7: 0.004%
9: 0.002%

poisson_variate(4.0): 67 nano
0: 1.847%
1: 7.35%
2: 14.704%
3: 19.386%
4: 19.67%
5: 15.676%
6: 10.363%
7: 5.809%
8: 3.047%
9: 1.329%
10: 0.519%
11: 0.204%
12: 0.075%
13: 0.013%
14: 0.007%
15: 0.001%

Random Dice 
-----------
d(10): 28 nano
1: 10.131%
2: 9.865%
3: 10.082%
4: 10.118%
5: 10.099%
6: 10.093%
7: 9.92%
8: 9.849%
9: 9.937%
10: 9.906%

dice(3, 6): 73 nano
3: 0.454%
4: 1.376%
5: 2.856%
6: 4.549%
7: 6.957%
8: 9.642%
9: 11.541%
10: 12.577%
11: 12.575%
12: 11.593%
13: 9.72%
14: 6.979%
15: 4.698%
16: 2.682%
17: 1.349%
18: 0.452%

dice(0, 6): 2 nano
0: 100%

ability_dice(4): 153 nano
3: 0.08%
4: 0.296%
5: 0.744%
6: 1.627%
7: 2.902%
8: 4.719%
9: 7.102%
10: 9.408%
11: 11.319%
12: 12.927%
13: 13.178%
14: 12.474%
15: 10.101%
16: 7.348%
17: 4.157%
18: 1.618%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 31 nano
0: 10.196%
1: 10.058%
2: 9.983%
3: 10.163%
4: 9.829%
5: 9.801%
6: 10.142%
7: 9.881%
8: 9.972%
9: 9.975%

front_linear(N): 15 nano
0: 19.109%
1: 16.989%
2: 14.917%
3: 13.091%
4: 10.966%
5: 8.873%
6: 7.081%
7: 5.022%
8: 2.977%
9: 0.975%

middle_linear(N): 21 nano
0: 1.928%
1: 6.159%
2: 9.882%
3: 14.134%
4: 18.102%
5: 17.937%
6: 13.874%
7: 9.945%
8: 5.991%
9: 2.048%

back_linear(N): 16 nano
0: 1.068%
1: 3.019%
2: 5.078%
3: 6.97%
4: 9.069%
5: 11.064%
6: 13.038%
7: 14.902%
8: 17.072%
9: 18.72%

quantum_linear(N): 39 nano
0: 7.396%
1: 8.607%
2: 10.002%
3: 11.432%
4: 12.551%
5: 12.75%
6: 11.303%
7: 9.894%
8: 8.648%
9: 7.417%

front_gauss(N): 22 nano
0: 63.201%
1: 23.231%
2: 8.547%
3: 3.155%
4: 1.177%
5: 0.423%
6: 0.16%
7: 0.064%
8: 0.024%
9: 0.018%

middle_gauss(N): 55 nano
0: 0.006%
1: 0.142%
2: 2.134%
3: 13.636%
4: 34.083%
5: 33.941%
6: 13.789%
7: 2.146%
8: 0.122%
9: 0.001%

back_gauss(N): 28 nano
0: 0.009%
1: 0.026%
2: 0.048%
3: 0.147%
4: 0.393%
5: 1.187%
6: 3.193%
7: 8.535%
8: 23.477%
9: 62.985%

quantum_gauss(N): 55 nano
0: 21.056%
1: 7.891%
2: 3.559%
3: 5.741%
4: 11.698%
5: 11.733%
6: 5.739%
7: 3.65%
8: 7.849%
9: 21.084%

front_poisson(N): 56 nano
0: 8.326%
1: 20.578%
2: 25.527%
3: 21.301%
4: 13.435%
5: 6.71%
6: 2.754%
7: 0.951%
8: 0.332%
9: 0.086%

middle_poisson(N): 78 nano
0: 4.291%
1: 10.321%
2: 13.367%
3: 11.968%
4: 10.074%
5: 9.976%
6: 12.059%
7: 13.429%
8: 10.283%
9: 4.232%

back_poisson(N): 58 nano
0: 0.078%
1: 0.288%
2: 1.032%
3: 2.705%
4: 6.646%
5: 13.465%
6: 21.327%
7: 25.698%
8: 20.466%
9: 8.295%

quantum_poisson(N): 84 nano
0: 4.181%
1: 10.235%
2: 13.406%
3: 12.192%
4: 9.971%
5: 10.113%
6: 11.935%
7: 13.353%
8: 10.367%
9: 4.247%

quantum_monty(N): 88 nano
0: 10.939%
1: 8.923%
2: 8.96%
3: 9.771%
4: 11.488%
5: 11.426%
6: 9.804%
7: 9.025%
8: 8.934%
9: 10.73%

F(-N) where N = 10
random_index(-N): 29 nano
-10: 9.962%
-9: 9.865%
-8: 10.19%
-7: 10.044%
-6: 10.06%
-5: 9.95%
-4: 9.94%
-3: 9.827%
-2: 10.08%
-1: 10.082%

front_linear(-N): 15 nano
-10: 19.005%
-9: 16.974%
-8: 15.224%
-7: 12.938%
-6: 11.057%
-5: 8.939%
-4: 6.766%
-3: 5.05%
-2: 3.015%
-1: 1.032%

middle_linear(-N): 23 nano
-10: 1.983%
-9: 6.056%
-8: 9.851%
-7: 13.957%
-6: 17.881%
-5: 18.005%
-4: 14.269%
-3: 9.966%
-2: 6.077%
-1: 1.955%

back_linear(-N): 18 nano
-10: 1.036%
-9: 2.949%
-8: 5.008%
-7: 6.993%
-6: 9.073%
-5: 11.011%
-4: 13.068%
-3: 15.023%
-2: 16.893%
-1: 18.946%

quantum_linear(-N): 38 nano
-10: 7.269%
-9: 8.653%
-8: 10.138%
-7: 10.997%
-6: 12.556%
-5: 12.593%
-4: 11.521%
-3: 10.065%
-2: 8.689%
-1: 7.519%

front_gauss(-N): 23 nano
-10: 63.302%
-9: 23.223%
-8: 8.559%
-7: 3.134%
-6: 1.118%
-5: 0.431%
-4: 0.14%
-3: 0.061%
-2: 0.025%
-1: 0.007%

middle_gauss(-N): 52 nano
-10: 0.002%
-9: 0.135%
-8: 2.187%
-7: 13.645%
-6: 34.06%
-5: 34.231%
-4: 13.564%
-3: 2.028%
-2: 0.143%
-1: 0.005%

back_gauss(-N): 23 nano
-10: 0.005%
-9: 0.022%
-8: 0.057%
-7: 0.16%
-6: 0.389%
-5: 1.148%
-4: 3.084%
-3: 8.6%
-2: 23.483%
-1: 63.052%

quantum_gauss(-N): 55 nano
-10: 21.216%
-9: 7.723%
-8: 3.632%
-7: 5.711%
-6: 11.866%
-5: 12.066%
-4: 5.524%
-3: 3.566%
-2: 7.763%
-1: 20.933%

front_poisson(-N): 59 nano
-10: 8.295%
-9: 20.584%
-8: 25.525%
-7: 21.386%
-6: 13.503%
-5: 6.483%
-4: 2.805%
-3: 0.997%
-2: 0.327%
-1: 0.095%

middle_poisson(-N): 90 nano
-10: 4.185%
-9: 10.296%
-8: 13.365%
-7: 12.056%
-6: 10.165%
-5: 10.007%
-4: 12.078%
-3: 13.197%
-2: 10.446%
-1: 4.205%

back_poisson(-N): 57 nano
-10: 0.097%
-9: 0.287%
-8: 0.996%
-7: 2.738%
-6: 6.629%
-5: 13.215%
-4: 21.63%
-3: 25.684%
-2: 20.497%
-1: 8.227%

quantum_poisson(-N): 87 nano
-10: 4.037%
-9: 10.468%
-8: 13.408%
-7: 11.967%
-6: 10.088%
-5: 9.893%
-4: 12.165%
-3: 13.466%
-2: 10.386%
-1: 4.122%

quantum_monty(-N): 86 nano
-10: 10.874%
-9: 9.113%
-8: 8.966%
-7: 9.667%
-6: 11.556%
-5: 11.421%
-4: 9.516%
-3: 9.047%
-2: 8.992%
-1: 10.848%

Random Float: timer only 
------------------------
canonical_variate(): 11 nano
uniform_real_variate(1.0, 10.0): 11 nano
exponential_variate(1.0): 11 nano
gamma_variate(1.0, 1.0): 23 nano
weibull_variate(1.0, 1.0): 12 nano
normal_variate(5, 2): 32 nano
lognormal_variate(1.6, 0.25): 32 nano
extreme_value_variate(0.0, 1.0): 12 nano
chi_squared_variate(1.0): 67 nano
cauchy_variate(0.0, 0.00005): 11 nano
fisher_f_variate(1.0, 1.0): 132 nano
student_t_variate(1.0): 121 nano
beta_variate(10.0, 1.0): 89 nano
pareto_variate(5.0): 11 nano
vonmises_variate(1.0, 1.0): 94 nano
triangular_variate(0.0, 10.0, 5.0): 11 nano

Clamp Tests 
-----------
clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano

============================
Total Time: 0.577564 seconds

```
