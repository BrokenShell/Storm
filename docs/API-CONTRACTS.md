# Storm 5 API contracts

This document defines the public behavior of Storm 5.0.1. The declarations in
`<Storm/Storm.hpp>` are the source for exact C++ signatures. Storm 4 names and
the `<Storm.hpp>` include path are not part of Storm 5.

## Types, version, and state

Storm 5 exposes no scalar type aliases. Signed integer and range values use
`std::int64_t`, unsigned bounds, seeds, and dice totals use `std::uint64_t`,
indices and counts use `std::size_t`, and canonical values use `double`.
`Storm::engine_type` is `std::mt19937_64`; `Storm::version` is the string
`"5.0.1"`.

`Storm::Generator` owns one engine. Copying a generator copies its state and
forks the sequence. `Generator(seed)` and `Generator::seed(seed)` are
deterministic for every seed, including zero. `Generator::engine()` returns the
owned engine by reference. Engine-reference accessors are `[[nodiscard]]`
because ignoring the returned reference has no useful effect.

Injected overloads accept a mutable engine reference and advance only that
engine. Convenience overloads use the calling thread's lazily created engine.
`Storm::thread_engine()` returns a mutable reference to that current-thread
engine; do not pass the reference to another thread or retain it after the
originating thread exits.

## Errors

- Invalid domains, zero sizes, zero steps, and empty intervals throw
  `std::invalid_argument`.
- Arithmetic or output that cannot be represented throws `std::overflow_error`.
- Validation occurs before sampling, so an invalid call does not intentionally
  advance the supplied engine.
- Operations that validate or request entropy are not unconditionally
  `noexcept`.

The clean core has no floating distribution parameters. Applications using
`<random>` distributions are responsible for validating finite values and the
distribution's documented domain before construction.

## Seeding and entropy

`Storm::seed(value)` deterministically seeds the calling thread's convenience
engine. It does not affect another thread, an existing `Generator`, or an
independently owned engine.

`Storm::reseed_from_entropy()` and `Generator::reseed_from_entropy()` gather
multiple `std::random_device` results and feed them through `std::seed_seq`.
They may throw. The C++ standard does not promise that `random_device` is
hardware-backed, nondeterministic, or cryptographically secure.

## Integer algorithms

### `uniform_integer(engine, low, high)`

- Samples the closed interval `[low, high]` uniformly.
- Requires `low <= high`; equal bounds return that value.
- Supports the complete signed interval from `LLONG_MIN` through `LLONG_MAX`.
- Does not use floating conversion, signed overflow, `abs(LLONG_MIN)`, or
  negation of `LLONG_MIN`.

### `uniform_unsigned(engine, low, high)`

- Samples the closed interval `[low, high]` uniformly.
- Requires `low <= high`; equal bounds return that value.
- Supports the complete unsigned domain.

### `uniform_index(engine, size)`

- Treats `size` as a nonnegative container size.
- Requires `size > 0`.
- Returns a uniform index in `[0, size)`.

### `random_range(engine, start, stop, step)`

- Uses directed, Python-like phase semantics; it never reorders bounds.
- A positive step requires `start < stop` and samples values
  `start + k * step < stop` uniformly.
- A negative step requires `start > stop` and samples values
  `start + k * step > stop` uniformly.
- A zero step or a direction that produces an empty sequence throws
  `std::invalid_argument`.
- Count and selected-value arithmetic is integral and supports full signed
  endpoints.

Each function also has a convenience overload with the same value contract
that uses the calling thread's engine.

## Dice algorithms

### `roll_die(engine, sides)`

- Requires `sides > 0`.
- Returns every value in `[1, sides]` with equal probability.

### `roll_dice(engine, rolls, sides)`

- Requires `sides > 0`.
- Zero rolls returns zero without drawing.
- One-sided dice return `rolls` immediately, including at `SIZE_MAX`.
- Otherwise returns the sum of `rolls` independent die draws.
- Throws `std::overflow_error` when the maximum possible total is not
  representable as `std::uint64_t`.

### `ability_dice(engine, dice_count)`

- Rolls `dice_count` six-sided dice and sums the highest three.
- Requires `dice_count >= 3`.
- Returns a value in `[3, 18]`.
- Keeps only the best three values, allocates no storage proportional to the
  count, and may stop once three sixes make the result final.

## Canonical floating algorithm

`canonical(engine)` returns a finite `double` in `[0.0, 1.0)`. It uses 53 bits
from the supplied engine, includes zero, and excludes one.

## Reproducibility

For identical `engine_type` state and inputs, exact output from Storm-owned
bounded integer and index algorithms is stable throughout major version 5.
This includes `uniform_integer`, `uniform_unsigned`, `uniform_index`, directed
`random_range`, and the bounded draws used by dice.

Storm does not promise exact cross-toolchain streams for C++ standard-library
distribution classes. Their transformations may differ between libstdc++,
libc++, MSVC's standard library, or library versions even though the raw
`std::mt19937_64` sequence is standardized.
