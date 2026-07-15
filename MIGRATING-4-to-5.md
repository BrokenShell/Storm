# Migrating from Storm 4 to Storm 5

Storm 5 is a correctness and packaging major release. It retains a deprecated
v4 facade where the old behavior can be defined safely, but it does not retain
undefined behavior, invalid standard-distribution parameters, incorrect
boundaries, or security marketing.

Do not replace a vendored v4 header without auditing exception translation and
the behavior changes below.

## Summary

| Storm 4 | Storm 5 |
| --- | --- |
| Top-level `Storm.hpp` | Canonical `<Storm/Storm.hpp>`; `<Storm.hpp>` forwards for compatibility |
| Hurricane shuffle/drop engine | `std::mt19937_64` directly as `Storm::engine_type` |
| Implicit TLS engine only | Owned `Storm::Generator`, injected-engine algorithms, and lazy TLS convenience |
| `Engine::seed(0)` requests entropy | `Storm::seed(0)` is deterministic |
| Seeding described as hardware | Separate, potentially throwing `Storm::reseed_from_entropy()` with no hardware guarantee |
| Silent clamps/sentinels/invalid distribution construction | Documented exceptions and finite/domain validation |
| Order-independent `GetInt::random_range` | Directed clean `Storm::random_range`; legacy facade preserves v4 phase |
| Signed dice/index conventions are the main API | Production API uses statistically honest sizes; signed forms are deprecated wrappers |
| Print-only demo/timing program | Dependency-free CTest gate and separate opt-in benchmarks |
| Non-commercial Creative Commons license | MIT License |

## Includes, namespace, and CMake

The namespace remains `Storm`.

Use the canonical include in new code:

```cpp
#include <Storm/Storm.hpp>
```

Existing code may continue to include `<Storm.hpp>`, which is a forwarding
compatibility header.

Link the interface target in both build-tree and installed-package consumers:

```cmake
target_link_libraries(my_target PRIVATE Storm::Storm)
```

Installed consumers use `find_package(Storm 5 CONFIG REQUIRED)`.

## Engine and seeding migration

Storm 5's production engine is `std::mt19937_64` directly. The Hurricane name
and its shuffle/drop composition are not part of the clean API. Storm makes no
cryptographic-strength claim.

Prefer explicit ownership:

```cpp
Storm::engine_type engine{1234};
auto value = Storm::uniform_integer(engine, -10, 10);
```

`Storm::Generator` provides an owned facade when an application wants to keep
the engine and operations together. Core algorithm overloads consume injected
engines by reference; copying an engine intentionally forks its state.

The convenience API uses a generator created lazily for each thread:

```cpp
Storm::seed(0); // deterministic; calling thread only
auto value = Storm::uniform_index(10);
```

Important changes:

- Seed zero is an ordinary deterministic seed.
- `Storm::seed(value)` changes only the current thread's TLS engine.
- It does not reseed existing `Generator` objects or engines in other threads.
- `Storm::reseed_from_entropy()` is the explicit entropy operation for the
  current thread and may throw.
- Entropy reseeding collects multiple `std::random_device` words and uses
  `std::seed_seq`.
- `std::random_device` is implementation-defined and is not promised to be
  hardware-backed, nondeterministic, or cryptographically secure.

The deprecated `Storm::Engine::seed` name may remain source-compatible, but it
follows the safe v5 deterministic-zero rule. Code that used zero to mean
entropy must call `Storm::reseed_from_entropy()` explicitly.

## Validation and exceptions

Storm 4 often clamped parameters or forwarded invalid values into standard
distributions. Storm 5 validates before sampling:

- `std::invalid_argument`: invalid domains, non-finite parameters, zero sizes,
  zero steps, and empty intervals
- `std::overflow_error`: required arithmetic or a result cannot be represented

Do not rely on unconditional `noexcept` from the v4 header. Catch exceptions at
language and ABI boundaries.

## Behavioral changes

| Area | Storm 5 behavior |
| --- | --- |
| `min_int()` | Returns `std::numeric_limits<long long>::min()`, not `-max()` |
| `seed(0)` | Deterministic |
| `uniform_index(0)` / zero legacy index | Throws `std::invalid_argument`; there is no valid index into an empty interval |
| `binomial_variate(0, p)` | Returns zero for valid `p` |
| Distribution parameters | Invalid, NaN, and infinite values are rejected before use |
| Integer boundaries | Full signed endpoints are handled without floating conversion or signed overflow |
| Canonical floating variate | `[0, 1)` |
| Dice totals | Unrepresentable totals throw `std::overflow_error` |
| Ability dice | Input is validated before conversion/allocation; no narrowing from an unchecked signed value |

Values around `2^53` no longer pass through `double` in integer/range
arithmetic. `LLONG_MIN` is never passed through signed `abs` or unary negation.

## Clean directed range

The clean `Storm::random_range(start, stop, step)` follows directed,
Python-like phase semantics:

- `step > 0` requires `start < stop` and selects from
  `start, start + step, ...` while the value is less than `stop`.
- `step < 0` requires `start > stop` and selects from
  `start, start + step, ...` while the value is greater than `stop`.
- A zero step or a direction that produces an empty sequence throws
  `std::invalid_argument`.
- If the count or selected value cannot be represented,
  `std::overflow_error` is thrown.

The bounds are not reordered by the clean API.

## Deprecated v4 range phase

`Storm::GetInt::random_range` retains the v4 order-independent interpretation
for migration. Let `low = min(start, stop)`, `high = max(start, stop)`, and
`stride = |step|`:

- A positive step selects `low + k * stride` in `[low, high)`.
- A negative step selects `high - k * stride` in `(low, high]`.
- Swapping `start` and `stop` does not change the set or phase.
- A zero step or equal bounds now throws `std::invalid_argument`; v4's
  arbitrary return for an empty interval is not preserved.
- Unrepresentable arithmetic throws `std::overflow_error`.

Examples:

```text
GetInt::random_range(1, 10,  2) -> {1, 3, 5, 7, 9}
GetInt::random_range(10, 1,  2) -> {1, 3, 5, 7, 9}
GetInt::random_range(1, 10, -2) -> {10, 8, 6, 4, 2}
GetInt::random_range(10, 1, -2) -> {10, 8, 6, 4, 2}
```

## Dice migration

New code should use size-based `roll_die` and `roll_dice`:

- A die size must be positive.
- `roll_die(sides)` is uniform on `[1, sides]`.
- A roll count may be zero, producing a total of zero.
- Totals are checked for representability.

Deprecated `Storm::GetInt::d` and `Storm::GetInt::dice` retain signed v4
contracts when the result is representable:

- `d(sides > 0)` returns `[1, sides]`.
- `d(0)` returns zero.
- `d(sides < 0)` returns `[sides, -1]`.
- A negative roll count negates the sum produced by the corresponding positive
  count; combining negative rolls and negative sides therefore produces a
  positive total.
- Any unrepresentable count, magnitude, or total throws
  `std::overflow_error`.

These wrappers exist for source migration, not as a recommendation to model a
physical die with negative sides.

## ZeroCool and legacy profiles

New code should use size-based `Storm::uniform_index(size)`, which is uniform
on `[0, size)` and rejects zero.

Deprecated signed ZeroCool wrappers retain their index sign contract for every
representable nonzero size:

- `F(n)` for `n > 0` returns an index in `[0, n)`.
- `F(n)` for `n < 0` returns an index in `[n, 0)`, matching Python's negative
  index range.
- `F(0)` throws `std::invalid_argument` because an empty sequence has no valid
  index.

The `front_*`, `middle_*`, `back_*`, `quantum_*`, and `quantum_monty` gauss,
poisson, linear, and Monty families are deprecated custom positional profiles.
Names such as “gauss” and “poisson” describe their historical recipes; they do
not claim an exact canonical Gaussian or Poisson probability mass function.
Only their tested output range/sign contracts are compatibility promises.

## Deprecated facade

The following v4 families remain deprecated through 5.x where they can be
implemented safely:

- `Storm::GetBool`
- `Storm::GetInt`
- `Storm::GetFloat`
- `Storm::GetIndex`
- `Storm::Engine::seed`
- `Storm::GearBox`
- `Storm::Meters`
- `Storm::get_version`

They may validate more strictly than v4 and may throw. Bugs, undefined
behavior, invalid distribution construction, and misleading engine claims are
not compatibility contracts.

## Reproducibility migration

Do not use old benchmark samples as golden sequences. Storm guarantees exact
output stability within major v5 only for Storm-owned bounded-integer and index
algorithms with the same engine state and inputs.

Streams produced through C++ standard-library distributions are not promised
to match between libstdc++, libc++, MSVC's standard library, library versions,
or toolchains. Write portable tests around domains and deterministic
properties, not implementation-specific `std` distribution samples.

## Fortuna compatibility record

This section records the audited state of Fortuna at the time Storm 5.0.0 was
prepared. Storm 5 releases independently. **Do not replace Fortuna's vendored
header as part of the Storm 5 release.**

Audited facts:

- Fortuna reports version 5.6.0 in `Fortuna.pyx`.
- Fortuna vendors `Storm.hpp` version 4.0.4.
- Its `cdef extern from "Storm.hpp"` block contains 57 Cython declarations.
- All 57 declarations lack `except +`.
- `fortuna_extras/multi_threading.py` uses `multiprocessing.Pool`; it is not a
  real thread-isolation test.
- Fortuna's seed documentation says software seeding can make all threads use
  one seed, which is not the v5 current-thread-only TLS contract.

Required order of work for a later Fortuna migration:

1. Add Cython C++ exception translation (`except +`, with any needed return
   type adjustments) for every declaration that can throw.
2. Build and test that binding change while Fortuna still vendors Storm 4.0.4.
3. Replace the vendored header only after exception translation is in place.
4. Update Python tests and documentation for:
   - the true `min_int`
   - deterministic seed zero and explicit entropy reseeding
   - zero-sized indices becoming errors
   - zero-trial binomial returning zero
   - invalid distribution domains, NaN, and infinity becoming errors
   - complete signed-boundary behavior
5. Preserve or deliberately migrate Python-visible dice, order-independent
   `random_range`, and signed ZeroCool contracts.
6. Replace the multiprocessing uniqueness heuristic with real thread tests for
   TLS isolation and current-thread seeding.
7. Correct the thread and entropy documentation.

Those validation and exception changes are Python-visible behavior changes.
Fortuna should treat the migration as a likely major release, preferably
Fortuna 6.0 rather than a silent 5.x header replacement.
