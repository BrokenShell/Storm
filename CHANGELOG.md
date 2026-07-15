# Changelog

All notable changes to Storm are documented here. Storm follows semantic
versioning. The 5.0.0 entry remains unreleased until the owner tags and
publishes it.

## [5.0.0] - Unreleased

### Added

- A clean C++20 API using `std::mt19937_64` directly through
  `Storm::engine_type`.
- `Storm::Generator`, injected-engine algorithm overloads, and lazy
  current-thread convenience overloads.
- Deterministic seeding for every value, including zero, plus separate
  potentially throwing entropy reseeding through multiple `random_device`
  words and `seed_seq`.
- Uniform signed, unsigned, and size-based index sampling; directed
  `random_range`; canonical `[0,1)` sampling; and size-based dice operations.
- Exact-output stability for Storm-owned bounded integer and index algorithms
  throughout major version 5.
- The canonical `<Storm/Storm.hpp>` include layout.
- A CMake `Storm::Storm` interface target with `add_subdirectory`, staged
  install, and `find_package` consumer coverage.
- Dependency-free contract, boundary, deterministic, TLS, multi-TU,
  self-contained-header, and statistical smoke tests.
- Strict-warning multi-compiler CI, ASan/UBSan, Linux TSan, package consumers,
  and a separate opt-in benchmark target.

### Changed

- Version is 5.0.0 across project, header, package metadata, and tests.
- Storm 5 is a clean source and include-path break from Storm 4.
- `seed(0)` is deterministic; entropy reseeding is explicit.
- TLS seeding affects only the calling thread.
- `random_range` is directed and Python-like.
- Dice and index APIs use nonnegative sizes rather than signed sentinel
  conventions.
- Invalid domains and empty intervals throw `std::invalid_argument`;
  unrepresentable arithmetic throws `std::overflow_error`.
- The default license is MIT.

### Fixed

- Integer and range algorithms handle full signed endpoints and values around
  `2^53` without floating conversion, signed overflow, `abs(LLONG_MIN)`, or
  negating `LLONG_MIN`.
- Canonical samples are always in `[0,1)`.
- Ability dice avoids unchecked narrowing and proportional allocation.
- Dice overflow is detected, and `roll_dice(SIZE_MAX, 1)` completes in constant
  time.

### Removed

- The complete Storm 4 facade: `GetBool`, `GetInt`, `GetFloat`, `GetIndex`,
  `Engine`, `GearBox`, `Meters`, `get_version`, signed dice/range/ZeroCool
  forms, and custom gauss/poisson/quantum/Monty recipes.
- The cosmetic `Float`, `Integer`, and `Unsigned` aliases; public signatures
  use fixed-width standard types directly.
- The top-level `<Storm.hpp>` include and forwarding headers.
- Hurricane-era engine aliases, composition, strength/security marketing, and
  the deprecation macro.
- Storm-owned wrappers around standard-library distributions. Applications
  should use the needed standard distribution with an injected engine.
- Claims that `std::random_device` is necessarily hardware-backed.
- The broken `compile-so.sh` workflow and unsafe `-ffast-math` or
  `-march=native` defaults.
- Print-only demos as release correctness gates.

### Fortuna handoff

Fortuna remains on its vendored Storm 4.0.4 header. It must add Cython
exception translation and follow `MIGRATING-4-to-5.md` before adopting the
throwing Storm 5 API. Storm does not edit or silently migrate Fortuna.

## [4.0.4]

Last Storm 4 header version known to be vendored by Fortuna when Storm 5.0.0
was prepared.
