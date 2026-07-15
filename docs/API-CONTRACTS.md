# Storm 5 API contracts

This document defines the public behavioral contract for Storm 5.0.0. The
header declarations remain the source for exact C++ signatures.

## Types and state

`Storm::engine_type` is `std::mt19937_64` used directly. It is deterministic
and non-cryptographic.

The compatibility aliases are `Storm::Float` (`double`), `Storm::Integer`
(`long long`), and `Storm::Unsigned` (`unsigned long long`). `Storm::version`
and deprecated `Storm::get_version()` report `5.0.0`.

`Storm::Generator` owns an engine. Copying a generator or an `engine_type`
copies its state and forks the sequence. Moving transfers that state according
to ordinary C++ value semantics.

`Generator(seed)` and `Generator::seed(seed)` are deterministic for every seed,
including zero. `Generator::engine()` returns the owned `engine_type` by
reference. `Generator::reseed_from_entropy()` follows the entropy contract
below and may throw.

Injected overloads accept a mutable engine by reference and advance exactly
that engine. They do not read or mutate Storm's TLS engine.

Convenience overloads without an engine use the calling thread's lazily
created engine. `Storm::thread_engine()` returns a mutable reference to that
same current-thread engine. A reference must not be passed to another thread or
used after its originating thread exits.

## Errors

Storm uses a uniform invalid-input policy:

- `std::invalid_argument` for invalid domains, non-finite floating parameters,
  zero sizes, zero steps, and empty intervals
- `std::overflow_error` when required arithmetic or an output cannot be
  represented by the public result type

Validation occurs before sampling. An invalid call does not intentionally
advance the supplied engine.

Operations that validate, allocate, request entropy, or invoke user code are
not unconditionally `noexcept`.

## Seeding and entropy

`Storm::seed(value)` deterministically seeds the calling thread's convenience
engine. Zero is an ordinary deterministic seed. Repeating the same seed in the
same thread resets that TLS stream.

`Storm::seed` does not affect:

- another thread's convenience engine
- an existing `Storm::Generator`
- an independently owned `Storm::engine_type`

`Storm::reseed_from_entropy()` reseeds the calling thread's engine using
multiple `std::random_device` results through `std::seed_seq`. It may throw an
exception from entropy acquisition. The C++ standard does not promise that
`std::random_device` is hardware-backed, nondeterministic, or cryptographically
secure.

For reproducible threaded code, explicitly construct and seed one engine per
logical stream.

## Integer algorithms

### `uniform_integer(engine, low, high)`

- Returns every representable integer in the closed interval `[low, high]`
  with equal probability.
- Requires `low <= high`; reversed bounds throw `std::invalid_argument`.
- Supports `low == high` and returns that value.
- Supports the complete signed interval from `LLONG_MIN` through `LLONG_MAX`.
- Does not convert bounds or interval widths through floating point.

The overload without `engine` has the same value contract and uses TLS.

### `uniform_unsigned(engine, low, high)`

- Returns every unsigned integer in the closed interval `[low, high]` with
  equal probability.
- Requires `low <= high`; reversed bounds throw `std::invalid_argument`.
- Supports equal bounds and the complete unsigned domain.
- Uses Storm-owned bounded-integer mapping.

### `uniform_index(engine, size)`

- `size` is a nonnegative size, not a signed Python index.
- Requires `size > 0`; zero throws `std::invalid_argument`.
- Returns a uniformly distributed index in `[0, size)`.
- Uses Storm-owned bounded-integer mapping.

### `random_range(engine, start, stop, step)`

- The interval is directed; bounds are not reordered.
- For `step > 0`, requires `start < stop` and samples uniformly from
  `start + k * step < stop`.
- For `step < 0`, requires `start > stop` and samples uniformly from
  `start + k * step > stop`.
- `step == 0`, equal bounds, or a direction producing an empty sequence throws
  `std::invalid_argument`.
- Count and selected-value arithmetic is exact integer arithmetic.
- An unrepresentable count, intermediate, or result throws
  `std::overflow_error`.

This is Python-like phase behavior. The deprecated
`Storm::GetInt::random_range` contract differs; see the compatibility section.

## Dice algorithms

### `roll_die(engine, sides)`

- Requires a positive size.
- Returns each integer in `[1, sides]` with equal probability.
- Zero is an invalid empty domain and throws `std::invalid_argument`.

### `roll_dice(engine, rolls, sides)`

- `rolls` is a count and `sides` is a positive size.
- Zero rolls returns zero without drawing.
- Otherwise returns the sum of `rolls` independent `roll_die(sides)` draws.
- If the total cannot be represented, throws `std::overflow_error`.
- The implementation does not silently narrow a size or count.

### `ability_dice(engine, rolls)`

- Models rolling `rolls` six-sided dice and summing the highest three.
- Requires at least three rolls; smaller counts throw `std::invalid_argument`.
- The count is size-based and is not silently clamped or narrowed. The
  implementation keeps only the best three rolls, does not allocate in
  proportion to the count, and may stop once three sixes make the result
  final.
- Every result is in `[3, 18]`.

## Floating canonical algorithm

### `canonical(engine)`

- Returns a finite `double` in `[0.0, 1.0)`.
- Zero is included and one is excluded.
- It never intentionally returns NaN or infinity.

## Distribution parameter domains

Deprecated v4 distribution names remain available where they are well-defined,
but Storm validates their inputs before constructing a standard distribution
or evaluating a custom recipe.

All floating parameters must be finite. Additional domains are:

| Operation | Valid domain |
| --- | --- |
| Bernoulli probability | `0 <= p <= 1` |
| Percent-true probability | `0 <= percent <= 100` |
| Binomial | `trials >= 0`, `0 <= p <= 1`; zero trials returns zero |
| Negative binomial | required successes `> 0`, `0 < p <= 1` |
| Geometric | `0 < p < 1` |
| Poisson | mean `> 0` |
| Uniform real | finite `low < high` |
| Exponential | rate `> 0` |
| Gamma | shape `> 0`, scale `> 0` |
| Weibull | shape `> 0`, scale `> 0` |
| Normal | standard deviation `> 0` |
| Log-normal | log standard deviation `> 0` |
| Extreme value | scale `> 0` |
| Chi-squared | degrees of freedom `> 0` |
| Cauchy | scale `> 0` |
| Fisher F | both degrees of freedom `> 0` |
| Student t | degrees of freedom `> 0` |
| Beta | alpha `> 0`, beta `> 0` |
| Pareto | alpha `> 0` |
| Von Mises | concentration `>= 0`; location finite |
| Triangular | `low < high` and `low <= mode <= high` |

Values outside these domains, including NaN and infinity, throw
`std::invalid_argument`. Storm does not silently clamp an invalid probability
into range.

For valid parameters, the named variates use these mathematical supports:

| Operation | Output support |
| --- | --- |
| Bernoulli / percent true | `false` or `true` |
| Binomial | integers in `[0, trials]` |
| Negative binomial / geometric / Poisson | nonnegative integers |
| Uniform real | `[low, high)` |
| Exponential / gamma / Weibull / chi-squared / Fisher F | nonnegative reals |
| Normal / extreme value / Cauchy / Student t | real line |
| Log-normal | nonnegative reals |
| Beta | `[0, 1]` |
| Pareto | `[1, +infinity)` |
| Von Mises | `[0, 2*pi)` |
| Triangular | `[low, high]` |

These are domain/support contracts, not promises that every mathematical real
is representable as `double`. Standard-library transformation details remain
subject to the reproducibility limitation below.

## Integer boundaries

Integer/range helpers do not:

- convert integral bounds through `double`
- compute a signed subtraction that can overflow
- call signed `abs` on `LLONG_MIN`
- negate `LLONG_MIN`

Intervals spanning `LLONG_MIN`, `LLONG_MAX`, or both are supported whenever
the selected value is representable. Arithmetic whose required mathematical
result is not representable throws `std::overflow_error`.

## Deprecated compatibility facade

The v4 `GetBool`, `GetInt`, `GetFloat`, `GetIndex`, `Engine`, `GearBox`,
`Meters`, and `get_version` families are deprecated. Their wrappers may throw
under v5 validation.

Retained scalar compatibility contracts include:

- `GetBool::bernoulli_variate(p)` and `percent_true(percent)` validate rather
  than clamp; their probability domains are listed above.
- `GetInt::uniform_int_variate(a, b)` and `uniform_uint_variate(a, b)` retain
  order-independent inclusive bounds using `min(a,b)` through `max(a,b)`.
- `GetInt::random_below(n > 0)` returns `[0,n)`;
  `random_below(n < 0)` returns `(n,0]`; zero throws
  `std::invalid_argument`.
- `GetInt::binomial_variate`, `negative_binomial_variate`,
  `geometric_variate`, and `poisson_variate` use the validated domains above.
- `GetInt::plus_or_minus`, `plus_or_minus_linear`, and
  `plus_or_minus_gauss` return within `[-|n|, |n|]` for representable
  magnitudes. `LLONG_MIN` throws when its positive magnitude is required.
- `GetInt::ability_dice(n)` retains v4's clamp to three through nine dice, but
  performs that clamp without narrowing an unchecked value. Its result remains
  `[3,18]`.
- `GetFloat::canonical_variate()` returns `[0,1)`. Other `GetFloat` functions
  use the parameter domains in the table above; `uniform_real_variate(a,b)`
  returns `[a,b)`.
- `Meters::min_int()` returns the true signed minimum. Other `Meters` accessors
  report the corresponding `numeric_limits` extrema or the nearest finite
  floating values immediately below and above zero.
- `GearBox::clamp(target,a,b)` retains unordered boundary arguments. Deprecated
  callback helpers propagate user exceptions and reject arithmetic that cannot
  be represented.

### v4 order-independent `random_range`

For `Storm::GetInt::random_range(start, stop, step)`, let
`low = min(start, stop)`, `high = max(start, stop)`, and `stride = |step|`:

- positive step: uniformly selects the `low`-anchored phase in `[low, high)`
- negative step: uniformly selects the `high`-anchored phase in `(low, high]`
- swapping `start` and `stop` does not change the phase
- zero step or equal bounds throws `std::invalid_argument`
- unrepresentable arithmetic throws `std::overflow_error`

### v4 signed dice

- `GetInt::d(n > 0)` returns `[1, n]`.
- `GetInt::d(0)` returns zero.
- `GetInt::d(n < 0)` returns `[n, -1]`.
- A negative `GetInt::dice` roll count negates the corresponding positive-count
  total.
- Unrepresentable counts, magnitudes, or totals throw
  `std::overflow_error`.

### v4 signed ZeroCool indices

For each retained legacy index profile:

- positive `n` returns `[0, n)`
- negative `n` returns `[n, 0)`
- zero throws `std::invalid_argument`

This includes representable signed endpoint inputs. The gauss, poisson,
quantum, and Monty profile names describe historical custom recipes. They are
not promises of exact canonical probability distributions. The compatibility
promise covers their tested range and sign behavior only.

## Reproducibility

With the same `engine_type` state and inputs, exact output from Storm-owned
bounded-integer and index algorithms is stable throughout major version 5.

Storm does not promise exact cross-toolchain streams for algorithms implemented
with C++ standard-library distribution classes. Those transformations may vary
between libstdc++, libc++, MSVC's standard library, or library versions even
though `std::mt19937_64` itself has a standardized raw sequence. Floating-point
evaluation may also vary by platform.

Do not compare exact `std` distribution sample streams in portable tests.
Compare documented domains, deterministic properties, and conservative
statistical behavior instead.
