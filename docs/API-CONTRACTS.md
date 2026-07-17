# Storm 5 API contracts

This document defines the public behavior of Storm 5. The declarations in
`<Storm/Storm.hpp>` are the source for exact C++ signatures. Storm 4 names and
the `<Storm.hpp>` include path are not part of Storm 5.

## Types, version, and state

Storm 5 exposes no scalar type aliases. Signed integer and range values use
`std::int64_t`, unsigned bounds, seeds, and dice totals use `std::uint64_t`,
indices and counts use `std::size_t`, and canonical values use `double`.
`Storm::engine_type` is `std::mt19937_64`; `Storm::version` is the string
`"5.0.3"`.

`Storm::PreparedWeightedIndex` and `Storm::PreparedCumulativeWeightedIndex`
own only prepared numeric selection state: a cumulative `double` boundary
table and its total. They do not own an engine, application values, Python
objects, callable resolution, locking, fork behavior, or process entropy.

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

The prepared weighted-index selectors reject invalid tables before any engine
is supplied. Applications directly using other `<random>` distributions
remain responsible for validating finite values and the distribution's
documented domain before construction.

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

## Stateful wide-index selection

### `wide_index_selector(engine, size)` and `selector(engine)`

- Construction requires `size > 0`, otherwise it throws `std::invalid_argument`
  before intentionally advancing the supplied engine.
- Construction creates a permutation of `[0, size)` with Fortuna 6.0.2's native
  Knuth-B schedule. For each `position` from `size - 2` down to zero, it draws
  `other` uniformly from the closed interval `[position, size - 1]` with
  `uniform_unsigned` and swaps those entries. It takes O(size) time and storage
  and may report ordinary allocation failures for populations that cannot be
  represented by available storage.
- The rotation width is `max(1, integer_sqrt(size))`. Each selection
  rejection-samples a `std::poisson_distribution<std::uint64_t>` with mean
  `rotation_width / 4.0` until the sample is below the width. The cursor begins
  at `size - 1`, moves cyclically backward by `1 + sample`, and returns the
  permuted index at that cursor. This is equivalent to positive
  `deque.rotate(1 + sample)` followed by `data[-1]`.
- Cursor arithmetic does not overflow `std::size_t`. Returned values are in
  `[0, size)`, and consecutive selections cannot repeat when `size > 1`.
- A size-one selector still samples the prepared Poisson distribution, then
  returns zero. Selection therefore consumes a variable number of engine
  values, including when rejected distances require another sample.
- Construction performs `size - 1` bounded unsigned draws and preserves their
  exact Storm engine schedule. A size-one construction consumes no engine
  values.
- The object owns its permutation, cursor, rotation width, and prepared
  distribution. It never owns or retains an engine, never uses process entropy
  or hidden global state, and has no thread-local convenience overload.
- Selection mutates the selector and the caller-owned engine. Concurrent use of
  either object requires external synchronization. Copying a selector copies
  its current prepared state; the copies subsequently advance independently
  when used with independent engines.
- Exact returned-index and post-operation engine equivalence is expected only
  on the same standard-library implementation and version. Storm does not
  promise cross-standard-library sequences for `std::poisson_distribution`.

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

## Prepared weighted-index distribution

### `PreparedWeightedIndex(weights)`

- Accepts an `std::initializer_list<double>` or an input range whose references
  are convertible to `double`.
- Requires a nonempty range of finite, nonnegative converted weights, at least
  one of which is positive. Weights are relative and need not be normalized.
- Builds cumulative weights from left to right in `O(n)` time and owns `O(n)`
  numeric storage. Construction accepts no engine and consumes no engine state.
- Throws `std::invalid_argument` for an empty range, a negative or non-finite
  weight, or an all-zero table. Throws `std::overflow_error` before an addition
  whose finite positive total is not representable as `double`.

### `PreparedCumulativeWeightedIndex(boundaries)`

- Accepts an `std::initializer_list<double>` or an input range whose references
  are convertible to `double`.
- Requires a nonempty range of finite, nonnegative, monotonically nondecreasing
  cumulative boundaries with a positive final boundary. Boundaries need not be
  normalized.
- Copies each converted `double` boundary directly into owned storage in
  `O(n)` time. It does not calculate adjacent differences, partial sums, or any
  other reconstruction. Construction accepts no engine and consumes no engine
  state.
- Equal adjacent boundaries represent zero-weight entries and are valid.
- Throws `std::invalid_argument` for an empty range, a negative or non-finite
  boundary, decreasing boundaries, or a final boundary of zero.

### `prepared(engine)` for either prepared selector

- Constructs a `std::uniform_real_distribution<double>` over `[0, total)` and
  draws using the explicitly supplied engine reference.
- Returns the index of the first cumulative boundary strictly greater than the
  draw using logarithmic search. Duplicate boundaries are skipped, so a
  zero-weight entry is never selected.
- Some standard-library implementations can round a floating result up to the
  upper endpoint for subnormal totals. Construction therefore prepares
  `nextafter(total, 0)`, the greatest representable `double` below the total.
  If a distribution result is not below `total`, selection substitutes that
  prepared value before searching. The effective draw is consequently always
  in `[0, total)`, including when `total` is `denorm_min()` and the prepared
  value is zero.
- Performs one standard-library distribution call and otherwise consumes no
  engine state. The prepared object remains unchanged and owns no engine.
- Selection is `O(log n)` and performs no allocation.

Relative-weight cumulative sums and the distribution operate in `double`.
Ordinary floating-point rounding therefore applies; a positive relative weight
too small to advance a much larger cumulative sum has no distinct representable
interval. The implementations do not normalize, renormalize, or replace the
supplied values.

## Reproducibility

For identical `engine_type` state and inputs, exact output from Storm-owned
bounded integer and index algorithms is stable throughout major version 5.
This includes `uniform_integer`, `uniform_unsigned`, `uniform_index`, directed
`random_range`, and the bounded draws used by dice.

Storm does not promise exact cross-toolchain streams for C++ standard-library
distribution classes. Their transformations may differ between libstdc++,
libc++, MSVC's standard library, or library versions even though the raw
`std::mt19937_64` sequence is standardized. This limitation applies to
both prepared weighted-index selectors, which deliberately preserve the
selected standard library's `std::uniform_real_distribution<double>` draw and
engine-consumption behavior.
