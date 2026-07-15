# Changelog

All notable changes to Storm are documented here. Storm follows semantic
versioning. The 5.0.0 entry remains unreleased until the owner tags and
publishes it.

## [5.0.0] - Unreleased

### Added

- A clean C++20 API using `std::mt19937_64` directly through
  `Storm::engine_type`.
- `Storm::Generator` for owned state and algorithm overloads that consume an
  injected engine by reference.
- Lazy thread-local convenience overloads and `Storm::thread_engine()`.
- A separate, potentially throwing `Storm::reseed_from_entropy()` operation
  that seeds from multiple `std::random_device` values through
  `std::seed_seq`.
- Storm-owned bounded-integer and index mapping with an exact-output stability
  commitment for major version 5.
- Canonical `<Storm/Storm.hpp>` include layout and a top-level `<Storm.hpp>`
  forwarding header.
- CMake `Storm::Storm` interface target, install/export rules, versioned package
  configuration, and isolated `find_package` consumer verification.
- Dependency-free CTest coverage for public contracts, invalid inputs,
  boundaries, deterministic seeding, TLS isolation, engine injection,
  multi-translation-unit use, and conservative statistical smoke checks.
- Strict-warning, sanitizer, multi-compiler, staged-install, package-consumer,
  and Linux thread-sanitizer workflows.
- An opt-in benchmark executable and documented methodology with consumed
  results and standard-library baselines.

### Changed

- Version is 5.0.0 across project, headers, package metadata, and runtime
  version accessors.
- `Storm::seed(0)` is deterministic. Entropy reseeding is no longer overloaded
  onto the zero seed value.
- Thread-local seeding affects only the calling thread.
- The clean `random_range` is directed and Python-like; the deprecated v4
  facade retains its order-independent phase behavior.
- The production dice and index API takes sizes and reports invalid empty
  domains rather than relying on signed sentinel conventions.
- Invalid domains and empty intervals throw `std::invalid_argument`.
  Unrepresentable arithmetic throws `std::overflow_error`. Floating inputs are
  checked for finiteness.
- The canonical unit variate contract is `[0, 1)`.
- Storm is licensed under the MIT License instead of Creative Commons
  Attribution-NonCommercial.

### Fixed

- `min_int()` now reports the true minimum signed integer.
- Zero-trial binomial sampling returns zero.
- Integer and range operations avoid floating conversion, signed overflow,
  `abs(LLONG_MIN)`, and negation of `LLONG_MIN`.
- Full signed endpoints, including intervals spanning the complete signed
  domain, are handled without undefined behavior.
- Ability-dice inputs no longer narrow before validation or allocate from an
  unchecked signed value.
- Dice summation detects unrepresentable totals.
- Floating distributions reject invalid, NaN, and infinite parameters before
  constructing or evaluating a distribution.

### Deprecated

- The v4 `GetBool`, `GetInt`, `GetFloat`, `GetIndex`, `Engine`, `GearBox`,
  `Meters`, and `get_version` facade.
- Negative dice, signed ZeroCool, and v4 order-independent `random_range`
  entry points. They remain defined for migration where their result is
  representable.
- Legacy gauss, poisson, quantum, and Monty positional profiles. These are
  historical custom profiles, not canonical named distributions.

### Removed

- The Hurricane engine composition and its strength/security marketing.
- Claims that `std::random_device` is necessarily hardware-backed.
- The broken `compile-so.sh` shared-library workflow and default assumptions
  requiring `-ffast-math` or `-march=native`.
- Print-only demo tests as the release correctness gate.

### Compatibility note

Fortuna remains on its vendored Storm 4.0.4 header. It must add Cython exception
translation and complete the checklist in `MIGRATING-4-to-5.md` before replacing
that header.

## [4.0.4]

Last v4 header version known to be vendored by Fortuna when Storm 5.0.0 was
prepared.
