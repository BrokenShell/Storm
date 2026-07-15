# Storm

Storm is a C++20, header-only random-number library built directly on
`std::mt19937_64`. Storm 5 has a deliberately small API for owned engines,
injected-engine algorithms, and lazy thread-local convenience.

Storm is **not a cryptographic random-number generator**. Do not use it for
keys, tokens, nonces, passwords, or adversarially observable decisions.

## Requirements

- A C++20 compiler
- CMake for supported build, test, install, and package workflows
- No runtime or mandatory network dependencies

## Include and CMake

Storm 5 has one public include path:

```cpp
#include <Storm/Storm.hpp>
```

The old `<Storm.hpp>` path and all Storm 4 compatibility namespaces were
removed. See [Migrating from Storm 4 to 5](MIGRATING-4-to-5.md).

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

`Storm::Storm` supplies the include path and C++20 requirement. Storm's own
strict warnings and sanitizer settings do not propagate to consumers.

## Engine ownership

`Storm::engine_type` is `std::mt19937_64`. Every algorithm has an overload that
takes an engine by reference, so state ownership is explicit and testable:

```cpp
#include <Storm/Storm.hpp>

int main() {
    Storm::Generator generator{42};
    const auto face = Storm::roll_die(generator.engine(), 6);
    const double unit = Storm::canonical(generator.engine());
    return face <= 6 && unit < 1.0 ? 0 : 1;
}
```

Use `Storm::Generator` to own a stream, or own an `engine_type` directly. Pass
the engine through application code and into these operations:

- `uniform_integer`
- `uniform_unsigned`
- `uniform_index`
- `random_range`
- `roll_die`
- `roll_dice`
- `ability_dice`
- `canonical`

Storm does not wrap the standard distribution catalog. Use a standard-library
distribution with an injected engine when its contract fits:

```cpp
#include <Storm/Storm.hpp>

#include <random>

int main() {
    Storm::Generator generator{42};
    std::normal_distribution<double> normal{0.0, 1.0};
    const double sample = normal(generator.engine());
    return sample == sample ? 0 : 1;
}
```

Validate application inputs before constructing a distribution. Exact streams
from standard-library distributions are not portable across implementations.

## Thread-local convenience

Overloads without an engine use a generator created lazily for the current
thread:

```cpp
Storm::seed(0); // deterministic; current thread only
const auto first = Storm::uniform_index(100);
Storm::seed(0);
const auto repeated = Storm::uniform_index(100);
```

`Storm::thread_engine()` exposes that current thread's engine. `Storm::seed`
never changes another thread's engine or an owned `Generator`.

`Storm::reseed_from_entropy()` is a separate, potentially throwing operation.
It gathers multiple `std::random_device` values and passes them through
`std::seed_seq`. The C++ standard does not guarantee that `random_device` is
hardware-backed, nondeterministic, or cryptographically secure.

For reproducible parallel work, seed a distinct owned engine for each logical
stream instead of depending on scheduling or TLS initialization order.

## Contracts and reproducibility

Invalid domains and empty intervals throw `std::invalid_argument`.
Unrepresentable arithmetic throws `std::overflow_error`. Integer and range
operations do not convert bounds through floating point or perform signed
overflow. `canonical` returns a finite value in `[0, 1)`.

For the same engine state and arguments, exact output from Storm-owned bounded
integer and index algorithms is stable within major version 5. Standard
distribution transformations are not promised identical across libstdc++,
libc++, MSVC's standard library, versions, or toolchains.

See [API contracts](docs/API-CONTRACTS.md) for the complete public contract.

## Build and test

```sh
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSTORM_BUILD_TESTS=ON \
  -DSTORM_WARNINGS_AS_ERRORS=ON
cmake --build build/debug --parallel
ctest --test-dir build/debug --output-on-failure
```

CI covers GCC, Clang, AppleClang, MSVC, Clang-Tidy, ASan/UBSan, Linux TSan,
staged installation, `find_package`, and `add_subdirectory` consumers.
Benchmarks are opt-in and are neither correctness tests nor hosted timing
gates; see the [benchmark methodology](docs/BENCHMARKS.md).

## Documentation

- [API contracts](docs/API-CONTRACTS.md)
- [Migrating from Storm 4 to 5](MIGRATING-4-to-5.md)
- [Changelog](CHANGELOG.md)
- [Benchmark methodology](docs/BENCHMARKS.md)
- [Contributing](CONTRIBUTING.md)
- [Security policy](SECURITY.md)

## License

Storm is copyright Robert Sharp and is available under the [MIT License](LICENSE).
