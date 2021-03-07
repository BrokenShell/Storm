#  Storm v3.4.4
### Random Number Toolkit
#### C++17 Header Only Library
#### Author: Robert Sharp
##### © 2020 Robert Sharp, all rights reserved.
<br>
<hr>
<br>

##### Project File List
- `Storm.hpp` complete core functionality.
- `MonkeyTimer.hpp` distribution/timer framework.
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
Storm 3.4.4
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

percent_true(25.0): 20 nano
0: 74.982%
1: 25.018%

bernoulli_variate(0.25): 20 nano
0: 75.233%
1: 24.767%


Random Integer 
--------------

random_below(10): 48 nano
0: 10.005%
1: 10.041%
2: 9.889%
3: 9.961%
4: 10.022%
5: 10.037%
6: 10.133%
7: 9.975%
8: 9.986%
9: 9.951%

uniform_int_variate(0, 9): 44 nano
0: 9.761%
1: 10.009%
2: 10.002%
3: 10.129%
4: 10.134%
5: 10.08%
6: 10.074%
7: 10.064%
8: 9.907%
9: 9.84%

random_range(0, 20, 2): 62 nano
0: 9.928%
2: 9.946%
4: 10.059%
6: 10.002%
8: 9.926%
10: 10.072%
12: 10.022%
14: 9.918%
16: 10.053%
18: 10.074%

random_range(0, 20, -2): 54 nano
2: 9.966%
4: 9.876%
6: 9.973%
8: 10.082%
10: 9.944%
12: 10.075%
14: 10.072%
16: 10.002%
18: 10.066%
20: 9.944%

random_range(0, -20, 2): 55 nano
-20: 10.057%
-18: 9.885%
-16: 10.103%
-14: 9.982%
-12: 10.119%
-10: 9.82%
-8: 9.911%
-6: 9.962%
-4: 10.036%
-2: 10.125%

random_range(0, -20, -2): 54 nano
-18: 9.876%
-16: 10.021%
-14: 10.184%
-12: 10.124%
-10: 10.071%
-8: 9.886%
-6: 9.895%
-4: 10.081%
-2: 9.96%
0: 9.902%

plus_or_minus(10): 36 nano
-10: 4.849%
-9: 4.771%
-8: 4.793%
-7: 4.752%
-6: 4.737%
-5: 4.688%
-4: 4.816%
-3: 4.705%
-2: 4.805%
-1: 4.711%
0: 4.738%
1: 4.803%
2: 4.768%
3: 4.727%
4: 4.812%
5: 4.777%
6: 4.762%
7: 4.767%
8: 4.694%
9: 4.707%
10: 4.818%

plus_or_minus_linear(10): 68 nano
-10: 0.824%
-9: 1.735%
-8: 2.497%
-7: 3.251%
-6: 4.12%
-5: 4.938%
-4: 5.942%
-3: 6.557%
-2: 7.517%
-1: 8.244%
0: 8.936%
1: 8.2%
2: 7.478%
3: 6.603%
4: 5.826%
5: 4.883%
6: 4.133%
7: 3.183%
8: 2.593%
9: 1.714%
10: 0.826%

plus_or_minus_gauss(10): 76 nano
-10: 0.093%
-9: 0.249%
-8: 0.561%
-7: 1.118%
-6: 2.131%
-5: 3.676%
-4: 5.742%
-3: 8.16%
-2: 10.378%
-1: 11.987%
0: 12.347%
1: 11.788%
2: 10.282%
3: 8.058%
4: 5.606%
5: 3.701%
6: 2.15%
7: 1.101%
8: 0.539%
9: 0.236%
10: 0.097%

binomial_variate(10, 0.5): 93 nano
0: 0.084%
1: 0.973%
2: 4.387%
3: 11.769%
4: 20.603%
5: 24.711%
6: 20.514%
7: 11.6%
8: 4.319%
9: 0.939%
10: 0.101%

negative_binomial_variate(5, 0.75): 83 nano
0: 23.807%
1: 29.859%
2: 22.016%
3: 13.081%
4: 6.522%
5: 2.793%
6: 1.203%
7: 0.436%
8: 0.181%
9: 0.07%
10: 0.018%
11: 0.012%
12: 0.002%

geometric_variate(0.75): 27 nano
0: 75.039%
1: 18.682%
2: 4.766%
3: 1.151%
4: 0.278%
5: 0.055%
6: 0.019%
7: 0.01%

poisson_variate(4.0): 74 nano
0: 1.834%
1: 7.352%
2: 14.675%
3: 19.804%
4: 19.428%
5: 15.534%
6: 10.461%
7: 5.828%
8: 2.891%
9: 1.363%
10: 0.549%
11: 0.192%
12: 0.063%
13: 0.019%
14: 0.005%
15: 0.002%


Random Dice 
-----------

d(10): 32 nano
1: 9.903%
2: 10.119%
3: 9.995%
4: 10.057%
5: 9.946%
6: 9.939%
7: 9.96%
8: 10.205%
9: 9.92%
10: 9.956%

dice(3, 6): 87 nano
3: 0.471%
4: 1.391%
5: 2.79%
6: 4.592%
7: 6.997%
8: 9.681%
9: 11.72%
10: 12.412%
11: 12.466%
12: 11.656%
13: 9.72%
14: 7.01%
15: 4.583%
16: 2.703%
17: 1.349%
18: 0.459%

dice(0, 6): 2 nano
0: 100%

ability_dice(4): 166 nano
3: 0.071%
4: 0.303%
5: 0.781%
6: 1.653%
7: 2.921%
8: 4.657%
9: 7.1%
10: 9.468%
11: 11.283%
12: 12.895%
13: 13.38%
14: 12.239%
15: 10.322%
16: 7.12%
17: 4.242%
18: 1.565%


Random Index: ZeroCool 
----------------------

F(N) where N = 10

random_index(N): 34 nano
0: 10.131%
1: 9.984%
2: 9.944%
3: 9.871%
4: 10.04%
5: 10.019%
6: 10.049%
7: 9.975%
8: 10.01%
9: 9.977%

front_linear(N): 17 nano
0: 19.072%
1: 17.077%
2: 15.094%
3: 12.994%
4: 10.874%
5: 8.88%
6: 6.984%
7: 5.064%
8: 2.972%
9: 0.989%

middle_linear(N): 20 nano
0: 2.039%
1: 5.9%
2: 10.033%
3: 13.774%
4: 18.154%
5: 17.951%
6: 14.108%
7: 10.089%
8: 5.89%
9: 2.062%

back_linear(N): 19 nano
0: 1.016%
1: 3.012%
2: 5.069%
3: 6.884%
4: 9.107%
5: 11.029%
6: 12.876%
7: 14.996%
8: 16.911%
9: 19.1%

quantum_linear(N): 43 nano
0: 7.417%
1: 8.713%
2: 9.93%
3: 11.352%
4: 12.603%
5: 12.653%
6: 11.418%
7: 9.917%
8: 8.79%
9: 7.207%

front_gauss(N): 23 nano
0: 62.911%
1: 23.531%
2: 8.567%
3: 3.175%
4: 1.141%
5: 0.441%
6: 0.141%
7: 0.066%
8: 0.018%
9: 0.009%

middle_gauss(N): 53 nano
0: 0.001%
1: 0.128%
2: 2.188%
3: 13.451%
4: 34.177%
5: 34.22%
6: 13.524%
7: 2.167%
8: 0.143%
9: 0.001%

back_gauss(N): 26 nano
0: 0.005%
1: 0.028%
2: 0.061%
3: 0.148%
4: 0.404%
5: 1.139%
6: 3.133%
7: 8.404%
8: 23.352%
9: 63.326%

quantum_gauss(N): 61 nano
0: 21.146%
1: 7.838%
2: 3.558%
3: 5.617%
4: 11.886%
5: 11.929%
6: 5.736%
7: 3.586%
8: 7.737%
9: 20.967%

front_poisson(N): 58 nano
0: 8.227%
1: 20.429%
2: 25.904%
3: 21.338%
4: 13.253%
5: 6.556%
6: 2.859%
7: 1.04%
8: 0.316%
9: 0.078%

middle_poisson(N): 78 nano
0: 4.294%
1: 10.283%
2: 13.418%
3: 12.064%
4: 10.073%
5: 10.005%
6: 11.924%
7: 13.383%
8: 10.385%
9: 4.171%

back_poisson(N): 58 nano
0: 0.093%
1: 0.332%
2: 0.976%
3: 2.784%
4: 6.796%
5: 13.13%
6: 21.497%
7: 25.731%
8: 20.414%
9: 8.247%

quantum_poisson(N): 88 nano
0: 4.09%
1: 10.462%
2: 13.321%
3: 12.085%
4: 9.947%
5: 9.989%
6: 12.192%
7: 13.435%
8: 10.288%
9: 4.191%

quantum_monty(N): 87 nano
0: 10.914%
1: 8.992%
2: 8.984%
3: 9.733%
4: 11.498%
5: 11.458%
6: 9.606%
7: 9.065%
8: 9.023%
9: 10.727%

F(-N) where N = 10

random_index(-N): 38 nano
-10: 9.981%
-9: 9.967%
-8: 9.973%
-7: 10.08%
-6: 10.099%
-5: 10.082%
-4: 10.088%
-3: 9.98%
-2: 9.803%
-1: 9.947%

front_linear(-N): 19 nano
-10: 18.832%
-9: 17.146%
-8: 14.983%
-7: 13.175%
-6: 11.125%
-5: 8.723%
-4: 6.877%
-3: 5.104%
-2: 3.021%
-1: 1.014%

middle_linear(-N): 22 nano
-10: 2.02%
-9: 6.186%
-8: 9.95%
-7: 13.852%
-6: 18.044%
-5: 17.752%
-4: 14.136%
-3: 10.056%
-2: 5.996%
-1: 2.008%

back_linear(-N): 19 nano
-10: 1.008%
-9: 2.882%
-8: 5.101%
-7: 7.116%
-6: 8.98%
-5: 11.132%
-4: 12.901%
-3: 15.115%
-2: 16.932%
-1: 18.833%

quantum_linear(-N): 45 nano
-10: 7.251%
-9: 8.676%
-8: 10.133%
-7: 11.457%
-6: 12.709%
-5: 12.545%
-4: 11.217%
-3: 10.07%
-2: 8.683%
-1: 7.259%

front_gauss(-N): 24 nano
-10: 63.214%
-9: 23.132%
-8: 8.684%
-7: 3.141%
-6: 1.154%
-5: 0.44%
-4: 0.143%
-3: 0.062%
-2: 0.022%
-1: 0.008%

middle_gauss(-N): 57 nano
-10: 0.002%
-9: 0.123%
-8: 2.141%
-7: 13.497%
-6: 34.265%
-5: 33.959%
-4: 13.702%
-3: 2.171%
-2: 0.134%
-1: 0.006%

back_gauss(-N): 24 nano
-10: 0.006%
-9: 0.021%
-8: 0.065%
-7: 0.143%
-6: 0.406%
-5: 1.149%
-4: 3.096%
-3: 8.474%
-2: 23.271%
-1: 63.369%

quantum_gauss(-N): 59 nano
-10: 21.211%
-9: 7.641%
-8: 3.698%
-7: 5.701%
-6: 11.932%
-5: 11.882%
-4: 5.615%
-3: 3.554%
-2: 7.798%
-1: 20.968%

front_poisson(-N): 68 nano
-10: 8.277%
-9: 20.496%
-8: 25.483%
-7: 21.375%
-6: 13.39%
-5: 6.733%
-4: 2.828%
-3: 1.008%
-2: 0.32%
-1: 0.09%

middle_poisson(-N): 87 nano
-10: 4.266%
-9: 10.398%
-8: 13.252%
-7: 12.018%
-6: 10.107%
-5: 9.971%
-4: 12.087%
-3: 13.298%
-2: 10.324%
-1: 4.279%

back_poisson(-N): 61 nano
-10: 0.076%
-9: 0.302%
-8: 0.953%
-7: 2.885%
-6: 6.641%
-5: 13.461%
-4: 21.212%
-3: 25.675%
-2: 20.582%
-1: 8.213%

quantum_poisson(-N): 94 nano
-10: 4.173%
-9: 10.525%
-8: 13.212%
-7: 12.123%
-6: 9.99%
-5: 10.1%
-4: 12.256%
-3: 13.21%
-2: 10.217%
-1: 4.194%

quantum_monty(-N): 97 nano
-10: 10.854%
-9: 9.015%
-8: 8.945%
-7: 9.718%
-6: 11.697%
-5: 11.582%
-4: 9.635%
-3: 8.877%
-2: 8.946%
-1: 10.731%


Random Float: timer only 
------------------------

canonical_variate(): 12 nano
uniform_real_variate(1.0, 10.0): 15 nano
exponential_variate(1.0): 16 nano
gamma_variate(1.0, 1.0): 24 nano
weibull_variate(1.0, 1.0): 12 nano
normal_variate(5, 2): 34 nano
lognormal_variate(1.6, 0.25): 34 nano
extreme_value_variate(0.0, 1.0): 12 nano
chi_squared_variate(1.0): 72 nano
cauchy_variate(0.0, 0.00005): 12 nano
fisher_f_variate(1.0, 1.0): 137 nano
student_t_variate(1.0): 123 nano
beta_variate(10.0, 1.0): 90 nano
pareto_variate(5.0): 12 nano
vonmises_variate(1.0, 1.0): 164 nano
triangular_variate(0.0, 10.0, 5.0): 13 nano


Clamp Tests 
-----------

clamp(150, 1, 100): 0 nano
smart_clamp(150, 1, 100): 0 nano


============================
Total Time: 0.637355 seconds

```
