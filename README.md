# Storm

Storm is a C++20, header-only random-number library built on
`std::mt19937_64`. Version 5 provides an explicit injected-engine API for
controlled state ownership and lazy thread-local convenience functions for
small applications.

Storm is **not a cryptographic random-number generator**. Do not use it for
keys, tokens, nonces, passwords, lotteries with adversarial participants, or
any other security-sensitive decision.

## Requirements

- A C++20 compiler
- CMake for the supported build, test, install, and package workflows
- No runtime or mandatory network dependencies

## Include and namespace

The canonical public include is:

```cpp
#include <Storm/Storm.hpp>
```

The top-level compatibility include remains available:

```cpp
#include <Storm.hpp>
```

Both expose the canonical `Storm` namespace. New code should use the clean v5
API rather than the deprecated v4 namespaces.

## Use with CMake

As a subdirectory:

```cmake
add_subdirectory(path/to/Storm)
target_link_libraries(my_program PRIVATE Storm::Storm)
```

From an installed package:

```cmake
find_package(Storm 5 CONFIG REQUIRED)
target_link_libraries(my_program PRIVATE Storm::Storm)
```

The `Storm::Storm` target supplies Storm's include path and C++20 requirement.
Storm's own strict warning settings are not propagated to consumers.

## Engine ownership

`Storm::engine_type` is `std::mt19937_64`. Algorithms have overloads that take
an engine by reference, making state ownership visible and testable:

```cpp
#include <Storm/Storm.hpp>

#include <cstdint>

int main() {
    Storm::engine_type engine{42};

    const std::int64_t face = Storm::uniform_integer(engine, 1, 6);
    const double unit = Storm::canonical(engine);
    return face >= 1 && face <= 6 && unit >= 0.0 && unit < 1.0 ? 0 : 1;
}
```

`Storm::Generator` is the owned-generator facade for callers that want to keep
an engine and its operations together. Prefer one owned generator per logical
random stream and pass it explicitly through application code.

The core operations are:

- `uniform_integer`
- `uniform_unsigned`
- `uniform_index`
- `random_range`
- `roll_die`
- `roll_dice`
- `ability_dice`
- `canonical`

See [API contracts](docs/API-CONTRACTS.md) for exact domains, endpoints, and
exceptions.

## Thread-local convenience

The overloads without an engine use a generator created lazily for the current
thread. `Storm::thread_engine()` provides direct access to that current
thread's engine.

```cpp
#include <Storm/Storm.hpp>

int main() {
    Storm::seed(0); // deterministic, and affects only this thread
    const auto first = Storm::uniform_index(100);

    Storm::seed(0);
    const auto repeated = Storm::uniform_index(100);
    return first == repeated ? 0 : 1;
}
```

`Storm::seed(value)` is deterministic for every value, including zero. It
seeds only the calling thread's convenience engine; it does not walk other
threads or configure engines that another object owns.

`Storm::reseed_from_entropy()` is the separate, potentially throwing operation
for reseeding the calling thread from implementation-provided
`std::random_device` values. Storm feeds multiple values through
`std::seed_seq`. The C++ standard does not guarantee that `random_device` is
hardware-backed, nondeterministic, or cryptographically secure.

For reproducible parallel work, construct and seed an explicit engine for each
worker instead of depending on thread scheduling or TLS initialization order.

## Reproducibility

For a fixed engine state, exact results from Storm-owned bounded-integer and
index algorithms are stable within major version 5. This promise does not
extend across future major versions.

The raw `std::mt19937_64` engine sequence is standardized, but C++ standard
library distributions may use implementation-specific transformations. Storm
therefore does not promise identical distribution streams across libstdc++,
libc++, MSVC's standard library, standard-library versions, or toolchains.
Floating-point results may also reflect ordinary platform differences.

## Validation

The clean API rejects invalid requests instead of silently clamping parameters
or selecting a sentinel:

- `std::invalid_argument` for invalid domains and empty intervals
- `std::overflow_error` when required integer arithmetic or a result cannot be
  represented
- finite checks for all floating-point parameters

Functions that validate, allocate, request entropy, or invoke user code are
not unconditionally `noexcept`.

## Build and test

```sh
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSTORM_BUILD_TESTS=ON \
  -DSTORM_WARNINGS_AS_ERRORS=ON
cmake --build build/debug --parallel
ctest --test-dir build/debug --output-on-failure
```

Sanitizer testing is available where the compiler and platform support it.
CI also checks strict warnings, header self-containment, multi-translation-unit
use, the installed package, and a separate consumer.

```sh
cmake -S . -B build/sanitize \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSTORM_BUILD_TESTS=ON \
  -DSTORM_WARNINGS_AS_ERRORS=ON \
  -DSTORM_ENABLE_ASAN_UBSAN=ON
cmake --build build/sanitize --parallel
ctest --test-dir build/sanitize --output-on-failure
```

Benchmarks are opt-in and are not correctness tests or hosted timing gates.
See the [benchmark methodology](docs/BENCHMARKS.md).

## Compatibility with Storm 4

The v4 `GetBool`, `GetInt`, `GetFloat`, `GetIndex`, `Engine`, `GearBox`,
`Meters`, and `get_version` surface remains only as a deprecated compatibility
facade where its behavior can be defined safely. Compatibility does not retain
undefined behavior, incorrect boundaries, silent domain repair, or unsupported
security claims.

The compatibility `random_range` retains the v4 order-independent phase
semantics. The clean v5 `random_range` uses a directed, Python-like interval.
Legacy gauss, poisson, quantum, and Monty profiles are custom historical
profiles, not claims of canonical probability distributions.

Read [Migrating from Storm 4 to 5](MIGRATING-4-to-5.md) before replacing a v4
header. This is particularly important for Cython users because C++ exceptions
must be translated at the binding boundary.

## Documentation

- [API contracts](docs/API-CONTRACTS.md)
- [Migrating from Storm 4 to 5](MIGRATING-4-to-5.md)
- [Changelog](CHANGELOG.md)
- [Benchmark methodology](docs/BENCHMARKS.md)
- [Contributing](CONTRIBUTING.md)
- [Security policy](SECURITY.md)

## License

Storm is copyright Robert Sharp and is available under the [MIT License](LICENSE).
