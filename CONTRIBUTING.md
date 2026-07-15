# Contributing to Storm

Thank you for improving Storm. Changes should preserve the library's small,
dependency-free public surface and make behavioral claims testable.

## Development requirements

- A C++20 compiler
- CMake
- Git

The normal workflow does not download dependencies. Do not add a mandatory
network fetch for tests or benchmarks without prior project-owner agreement.

## Configure and test

Run the strict Debug gate:

```sh
cmake -S . -B build/debug \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSTORM_BUILD_TESTS=ON \
  -DSTORM_WARNINGS_AS_ERRORS=ON
cmake --build build/debug --parallel
ctest --test-dir build/debug --output-on-failure
```

Run the sanitizer configuration on supported GCC and Clang platforms. If the
project's sanitizer option is unavailable on a platform, record that as an
environment limitation rather than silently treating it as a pass.

```sh
cmake -S . -B build/sanitize \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSTORM_BUILD_TESTS=ON \
  -DSTORM_WARNINGS_AS_ERRORS=ON \
  -DSTORM_ENABLE_ASAN_UBSAN=ON
cmake --build build/sanitize --parallel
ctest --test-dir build/sanitize --output-on-failure
```

Before submitting a release-facing change, also perform a Release install to a
fresh staging prefix and configure, build, and run a separate project using:

```cmake
find_package(Storm 5 CONFIG REQUIRED)
target_link_libraries(consumer PRIVATE Storm::Storm)
```

The consumer must not inherit Storm's warnings-as-errors or sanitizer flags.

## Test expectations

Tests are dependency-free CTest executables and must return nonzero on failure.
Add deterministic coverage for each changed contract. Depending on the change,
that includes:

- public-header self-containment and multi-translation-unit ODR use
- deterministic seed zero and injected engine state
- TLS isolation with real threads
- invalid arguments, NaN, and infinity
- `LLONG_MIN`, `LLONG_MAX`, complete signed intervals, and values around
  `2^53`
- empty ranges, zero-sized indices, and dice overflow
- positive and negative legacy range/index signs
- conservative deterministic statistical smoke tests
- project, header, package, and runtime version consistency

Statistical tests must use fixed seeds, broad deterministic tolerances, and a
sample size justified by the property being checked. They must not be timing
tests or flaky assertions that occasionally pass.

## Code style and API changes

- Keep the canonical namespace `Storm` and include `<Storm/Storm.hpp>`.
- Prefer small, self-contained headers and named detail helpers.
- Pass algorithm engines by reference; do not hide new process-global state.
- Avoid signed overflow, floating conversion for integer range arithmetic,
  `abs(LLONG_MIN)`, and negation of `LLONG_MIN`.
- Validate floating parameters for finiteness and domain before use.
- Use `std::invalid_argument` for invalid domains or empty intervals and
  `std::overflow_error` for unrepresentable arithmetic.
- Do not add unconditional `noexcept` when validation, allocation, entropy, or
  a user callback can fail.
- Do not describe an engine as secure, stronger, hardware-backed, or random
  without evidence that supports the exact statement.
- Do not promise cross-standard-library identity for `std` distributions.

New public behavior needs an API-contract update and a changelog entry.
Breaking behavior also needs a migration-guide entry.

## Benchmarks

Benchmarks are opt-in and separate from correctness tests. Follow
`docs/BENCHMARKS.md`: consume every result, compare an equivalent standard
baseline, disclose the environment, and do not add hosted timing thresholds.

## Pull request checklist

- [ ] The diff is scoped and contains no build artifacts.
- [ ] New or changed public contracts are documented and tested.
- [ ] Strict Debug tests pass.
- [ ] Sanitizer tests pass where supported, or the limitation is classified.
- [ ] Release installation and a separate package consumer pass.
- [ ] `git diff --check` is clean.
- [ ] Benchmarks make no claim beyond the recorded methodology and data.
- [ ] Version metadata remains consistent.
- [ ] Existing authorship and the MIT license are preserved.

Do not tag or publish a release as part of an ordinary pull request.

## Release-candidate verification

Release preparation is an owner-directed step after review; a feature pull
request must not create a tag or publish an archive. For a release candidate:

1. Confirm that the project, public headers, package configuration, changelog,
   and runtime version accessor all report the same version.
2. Run the strict Debug, supported sanitizer, Release install, and isolated
   package-consumer checks from clean build directories.
3. Create a source archive from the candidate commit, extract it outside the
   repository, and repeat configure/build/test/install from the extracted tree.
   This catches dependencies on Git metadata, ignored files, or developer-only
   paths.
4. Inspect the archive inventory. It must contain the license, public headers,
   CMake package files, tests, and release documentation, and must not contain
   build products, credentials, local caches, or editor state.
5. Record compiler/platform coverage and classify any infrastructure-only gap.
6. Replace `Unreleased` with the release date only when the owner approves the
   release, then tag and publish from the reviewed commit.
