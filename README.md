#  Storm v3.5.0
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
  - `GetFloat` long double variant namespace
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
Storm Version 3.5.0
===========

Min/Max Tests 
-------------
Minimum Integer: -9223372036854775807
Maximum Integer:  9223372036854775807
Minimum Float: -1.18973e+4932
Maximum Float:  1.18973e+4932
Minimum Below: -3.6452e-4951
Minimum Above:  3.6452e-4951

Random Boolean 
--------------
percent_true(25.0): 21 nano
0: 75.21%
1: 24.79%

bernoulli_variate(0.25): 19 nano
0: 74.848%
1: 25.152%

Random Integer 
--------------
random_below(10): 42 nano
0: 10.214%
1: 10.041%
2: 9.958%
3: 9.872%
4: 9.98%
5: 10.001%
6: 9.891%
7: 10.121%
8: 9.934%
9: 9.988%

uniform_int_variate(0, 9): 35 nano
0: 10.041%
1: 9.905%
2: 9.958%
3: 9.993%
4: 9.982%
5: 10.006%
6: 10.106%
7: 9.837%
8: 10.03%
9: 10.142%

random_range(0, 20, 2): 44 nano
0: 9.846%
2: 10.06%
4: 10.055%
6: 9.985%
8: 9.933%
10: 9.958%
12: 10.152%
14: 9.924%
16: 10.019%
18: 10.068%

random_range(0, 20, -2): 50 nano
2: 9.936%
4: 10.009%
6: 9.972%
8: 9.849%
10: 10.037%
12: 10.145%
14: 9.96%
16: 9.989%
18: 9.95%
20: 10.153%

random_range(0, -20, 2): 43 nano
-20: 9.867%
-18: 9.997%
-16: 9.993%
-14: 9.919%
-12: 10.055%
-10: 9.954%
-8: 10.119%
-6: 10.131%
-4: 10.079%
-2: 9.886%

random_range(0, -20, -2): 41 nano
-18: 10.017%
-16: 9.806%
-14: 10.023%
-12: 10.059%
-10: 9.955%
-8: 10.083%
-6: 9.999%
-4: 10.009%
-2: 10.018%
0: 10.031%

plus_or_minus(10): 31 nano
-10: 4.684%
-9: 4.765%
-8: 4.786%
-7: 4.816%
-6: 4.807%
-5: 4.627%
-4: 4.728%
-3: 4.701%
-2: 4.763%
-1: 4.931%
0: 4.694%
1: 4.77%
2: 4.787%
3: 4.673%
4: 4.774%
5: 4.604%
6: 4.808%
7: 4.717%
8: 4.811%
9: 4.895%
10: 4.859%

plus_or_minus_linear(10): 48 nano
-10: 0.857%
-9: 1.643%
-8: 2.438%
-7: 3.226%
-6: 4.031%
-5: 5.005%
-4: 5.705%
-3: 6.681%
-2: 7.57%
-1: 8.184%
0: 9.129%
1: 8.352%
2: 7.498%
3: 6.599%
4: 5.872%
5: 4.911%
6: 4.084%
7: 3.28%
8: 2.449%
9: 1.633%
10: 0.853%

plus_or_minus_gauss(10): 85 nano
-10: 0.106%
-9: 0.221%
-8: 0.532%
-7: 1.18%
-6: 2.164%
-5: 3.702%
-4: 5.795%
-3: 8.042%
-2: 10.069%
-1: 11.865%
0: 12.372%
1: 11.981%
2: 10.255%
3: 8.158%
4: 5.69%
5: 3.695%
6: 2.156%
7: 1.151%
8: 0.518%
9: 0.244%
10: 0.104%

binomial_variate(10, 0.5): 90 nano
0: 0.079%
1: 0.931%
2: 4.368%
3: 11.915%
4: 20.407%
5: 24.581%
6: 20.525%
7: 11.64%
8: 4.437%
9: 1.015%
10: 0.102%

negative_binomial_variate(5, 0.75): 74 nano
0: 23.54%
1: 29.672%
2: 22.277%
3: 13.056%
4: 6.561%
5: 2.976%
6: 1.164%
7: 0.471%
8: 0.181%
9: 0.065%
10: 0.022%
11: 0.01%
12: 0.003%
14: 0.001%
15: 0.001%

geometric_variate(0.75): 22 nano
0: 75.008%
1: 18.74%
2: 4.705%
3: 1.167%
4: 0.297%
5: 0.06%
6: 0.019%
7: 0.003%
8: 0.001%

poisson_variate(4.0): 71 nano
0: 1.754%
1: 7.272%
2: 14.504%
3: 19.6%
4: 19.721%
5: 15.847%
6: 10.269%
7: 5.834%
8: 3.011%
9: 1.372%
10: 0.519%
11: 0.206%
12: 0.062%
13: 0.024%
14: 0.004%
15: 0.001%

Random Dice 
-----------
d(10): 26 nano
1: 10.03%
2: 9.987%
3: 10.031%
4: 9.874%
5: 9.942%
6: 9.997%
7: 10.002%
8: 9.993%
9: 9.893%
10: 10.251%

dice(3, 6): 72 nano
3: 0.463%
4: 1.405%
5: 2.76%
6: 4.566%
7: 7.007%
8: 9.769%
9: 11.649%
10: 12.518%
11: 12.419%
12: 11.556%
13: 9.737%
14: 6.845%
15: 4.616%
16: 2.823%
17: 1.373%
18: 0.494%

dice(0, 6): 2 nano
0: 100%

ability_dice(4): 166 nano
3: 0.073%
4: 0.315%
5: 0.789%
6: 1.63%
7: 2.961%
8: 4.875%
9: 7.066%
10: 9.483%
11: 11.39%
12: 12.919%
13: 13.236%
14: 12.285%
15: 9.901%
16: 7.314%
17: 4.178%
18: 1.585%

Random Index: ZeroCool 
----------------------
F(N) where N = 10
random_index(N): 31 nano
0: 9.846%
1: 9.916%
2: 10.062%
3: 9.908%
4: 10.028%
5: 10.038%
6: 10.023%
7: 10.151%
8: 9.861%
9: 10.167%

front_linear(N): 17 nano
0: 19.059%
1: 16.812%
2: 15.109%
3: 12.786%
4: 10.968%
5: 9.058%
6: 7.177%
7: 4.984%
8: 3.084%
9: 0.963%

middle_linear(N): 24 nano
0: 2.096%
1: 6.02%
2: 9.927%
3: 14.144%
4: 18.005%
5: 18.035%
6: 13.881%
7: 9.977%
8: 5.94%
9: 1.975%

back_linear(N): 16 nano
0: 0.975%
1: 2.968%
2: 4.974%
3: 7.053%
4: 9.086%
5: 10.875%
6: 13.126%
7: 15.126%
8: 16.882%
9: 18.935%

quantum_linear(N): 41 nano
0: 7.308%
1: 8.703%
2: 10.179%
3: 11.403%
4: 12.648%
5: 12.658%
6: 11.296%
7: 10.001%
8: 8.567%
9: 7.237%

front_gauss(N): 49 nano
0: 63.413%
1: 23.074%
2: 8.596%
3: 3.139%
4: 1.127%
5: 0.391%
6: 0.166%
7: 0.06%
8: 0.024%
9: 0.01%

middle_gauss(N): 71 nano
0: 0.004%
1: 0.134%
2: 2.203%
3: 13.639%
4: 33.857%
5: 34.263%
6: 13.658%
7: 2.117%
8: 0.123%
9: 0.002%

back_gauss(N): 55 nano
0: 0.006%
1: 0.017%
2: 0.064%
3: 0.14%
4: 0.405%
5: 1.153%
6: 3.103%
7: 8.53%
8: 23.304%
9: 63.278%

quantum_gauss(N): 83 nano
0: 21.058%
1: 7.701%
2: 3.542%
3: 5.535%
4: 12.142%
5: 11.92%
6: 5.654%
7: 3.652%
8: 7.732%
9: 21.064%

front_poisson(N): 56 nano
0: 8.168%
1: 20.521%
2: 25.788%
3: 21.232%
4: 13.371%
5: 6.648%
6: 2.828%
7: 1.01%
8: 0.358%
9: 0.076%

middle_poisson(N): 80 nano
0: 4.213%
1: 10.564%
2: 13.3%
3: 12.129%
4: 9.949%
5: 9.82%
6: 11.923%
7: 13.338%
8: 10.55%
9: 4.214%

back_poisson(N): 57 nano
0: 0.108%
1: 0.293%
2: 0.938%
3: 2.789%
4: 6.615%
5: 13.373%
6: 21.461%
7: 25.818%
8: 20.339%
9: 8.266%

quantum_poisson(N): 97 nano
0: 4.124%
1: 10.48%
2: 13.593%
3: 12.127%
4: 10.011%
5: 9.896%
6: 11.944%
7: 13.29%
8: 10.355%
9: 4.18%

quantum_monty(N): 93 nano
0: 10.772%
1: 8.96%
2: 9.127%
3: 9.8%
4: 11.567%
5: 11.464%
6: 9.578%
7: 9.043%
8: 8.994%
9: 10.695%

F(-N) where N = 10
random_index(-N): 31 nano
-10: 9.94%
-9: 9.92%
-8: 10%
-7: 9.997%
-6: 10.009%
-5: 9.844%
-4: 10.092%
-3: 10.095%
-2: 10.172%
-1: 9.931%

front_linear(-N): 18 nano
-10: 19.038%
-9: 17.119%
-8: 14.993%
-7: 12.864%
-6: 11.097%
-5: 8.942%
-4: 6.896%
-3: 4.98%
-2: 3.036%
-1: 1.035%

middle_linear(-N): 23 nano
-10: 2.004%
-9: 5.919%
-8: 9.967%
-7: 13.977%
-6: 17.92%
-5: 17.91%
-4: 14.213%
-3: 10.074%
-2: 6.042%
-1: 1.974%

back_linear(-N): 22 nano
-10: 1.014%
-9: 3.019%
-8: 5.031%
-7: 6.833%
-6: 9.056%
-5: 10.947%
-4: 13.157%
-3: 14.827%
-2: 17.038%
-1: 19.078%

quantum_linear(-N): 46 nano
-10: 7.423%
-9: 8.764%
-8: 9.899%
-7: 11.142%
-6: 12.536%
-5: 12.858%
-4: 11.506%
-3: 9.94%
-2: 8.711%
-1: 7.221%

front_gauss(-N): 51 nano
-10: 63.481%
-9: 23.065%
-8: 8.458%
-7: 3.135%
-6: 1.157%
-5: 0.452%
-4: 0.169%
-3: 0.059%
-2: 0.014%
-1: 0.01%

middle_gauss(-N): 72 nano
-10: 0.002%
-9: 0.152%
-8: 2.094%
-7: 13.662%
-6: 34.289%
-5: 34.113%
-4: 13.444%
-3: 2.123%
-2: 0.117%
-1: 0.004%

back_gauss(-N): 52 nano
-10: 0.011%
-9: 0.024%
-8: 0.06%
-7: 0.15%
-6: 0.415%
-5: 1.152%
-4: 3.193%
-3: 8.474%
-2: 23.018%
-1: 63.503%

quantum_gauss(-N): 88 nano
-10: 20.849%
-9: 7.72%
-8: 3.565%
-7: 5.748%
-6: 12.099%
-5: 11.92%
-4: 5.748%
-3: 3.592%
-2: 7.949%
-1: 20.81%

front_poisson(-N): 62 nano
-10: 8.309%
-9: 20.407%
-8: 25.609%
-7: 21.537%
-6: 13.328%
-5: 6.616%
-4: 2.778%
-3: 1.032%
-2: 0.295%
-1: 0.089%

middle_poisson(-N): 85 nano
-10: 4.115%
-9: 10.534%
-8: 13.391%
-7: 12.149%
-6: 9.969%
-5: 10.102%
-4: 12.039%
-3: 13.264%
-2: 10.319%
-1: 4.118%

back_poisson(-N): 67 nano
-10: 0.074%
-9: 0.288%
-8: 0.952%
-7: 2.815%
-6: 6.756%
-5: 13.326%
-4: 21.325%
-3: 25.616%
-2: 20.527%
-1: 8.321%

quantum_poisson(-N): 96 nano
-10: 4.132%
-9: 10.33%
-8: 13.431%
-7: 12.222%
-6: 9.922%
-5: 10.197%
-4: 11.795%
-3: 13.291%
-2: 10.438%
-1: 4.242%

quantum_monty(-N): 104 nano
-10: 10.856%
-9: 8.983%
-8: 9.077%
-7: 9.71%
-6: 11.456%
-5: 11.371%
-4: 9.645%
-3: 8.956%
-2: 9.085%
-1: 10.861%

Random Float: timer only 
------------------------
canonical_variate(): 12 nano
uniform_real_variate(1.0, 10.0): 15 nano
exponential_variate(1.0): 12 nano
gamma_variate(1.0, 1.0): 35 nano
weibull_variate(1.0, 1.0): 12 nano
normal_variate(5, 2): 37 nano
lognormal_variate(1.6, 0.25): 45 nano
extreme_value_variate(0.0, 1.0): 12 nano
chi_squared_variate(1.0): 120 nano
cauchy_variate(0.0, 0.00005): 14 nano
fisher_f_variate(1.0, 1.0): 253 nano
student_t_variate(1.0): 192 nano
beta_variate(10.0, 1.0): 157 nano
pareto_variate(5.0): 12 nano
vonmises_variate(1.0, 1.0): 189 nano
triangular_variate(0.0, 10.0, 5.0): 12 nano

Clamp Tests 
-----------
clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano

============================
Total Time: 0.688757 seconds

```
