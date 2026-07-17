# Changelog

All notable changes to Storm are documented here. Storm follows semantic
versioning.

## [5.0.3] - 2026-07-17

### Added

- `Storm::PreparedCumulativeWeightedIndex`, a validated prepared cumulative
  boundary table that preserves supplied `double` boundaries directly and uses
  the same logarithmic strict-boundary selection schedule as
  `PreparedWeightedIndex`.
- Deterministic equivalence coverage proving relative weights and their
  cumulative boundaries return the same indexes and advance identically seeded
  engines in lockstep.

## [5.0.2] - 2026-07-16

### Added

- `Storm::PreparedWeightedIndex`, a validated prepared relative-weight table
  that owns no engine and performs logarithmic strict-boundary selection with
  an explicitly supplied engine.
- Deterministic reference-equivalence, validation, zero-weight, engine-state,
  boundary, and conservative statistical coverage for weighted selection.
- Opt-in consumed Release benchmarks comparing logarithmic and linear prepared
  selection for 4, 100, and 1000 entries without hosted timing thresholds.
- `Storm::wide_index_selector`, a stateful, engine-external wide-index schedule
  that preserves Fortuna 6.0.2's native Knuth-B permutation, unsigned Poisson
  distance, and positive-rotation selection schedule.
- Deterministic same-toolchain reference equivalence, engine-state, bounds,
  coverage, no-repeat, and broad marginal-uniformity tests for the selector.
- An opt-in Release benchmark comparing cursor selection with an equivalent
  compositional physical-rotation reference, including consumed construction
  and draw checksums without timing gates.

### Fixed

- Weighted selection now brings a standard-library draw rounded to the total
  down to `nextafter(total, 0)` before strict-boundary lookup, preventing an
  out-of-range index for subnormal totals and tables with trailing zeros.

### Changed

- Ability-dice rolls now maintain their fixed-size sorted result with direct
  insertion instead of re-sorting after every draw, while preserving returned
  values and exact engine advancement.

## [5.0.1] - 2026-07-15

### Added

- A checked-in Clang-Tidy policy for the public header and a dedicated
  Clang-Tidy 18 CI gate.
- An opt-in `STORM_ENABLE_CLANG_TIDY` CMake configuration for reproducing the
  repository lint policy locally.

### Changed

- Version is 5.0.1 across project, header, package metadata, documentation,
  and tests.
- Engine accessors now use `[[nodiscard]]` so accidentally ignored references
  are diagnosed.
- Ability-dice bookkeeping and its final sum now remain in `std::uint64_t`
  throughout, making the arithmetic domain explicit.
- Cast-initialized locals and sorting use the configured modern C++20 style.
- CI uses the Node.js 24-based checkout action, eliminating the hosted
  Node.js 20 deprecation annotation.

### Documented

- The predictable default thread-local seed is intentionally retained and
  narrowly suppressed from `cert-msc51-cpp`: deterministic seed zero is part
  of Storm's public reproducibility contract, not a security claim.

## [5.0.0] - Superseded before release

The 5.0.0 implementation was merged but was not tagged or published. Version
5.0.1 supersedes it as the first Storm 5 release.

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

## [4.0.4]

Final Storm 4 release before the clean Storm 5 break.
