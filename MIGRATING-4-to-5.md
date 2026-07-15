# Migrating from Storm 4 to Storm 5

Storm 5 is a clean break. It does not provide a deprecated facade, a
`<Storm.hpp>` forwarding include, or source compatibility for Storm 4.
Applications should migrate intentionally instead of replacing the old header
in place.

## Release-level changes

| Storm 4 | Storm 5 |
| --- | --- |
| `<Storm.hpp>` | `<Storm/Storm.hpp>` only |
| Hurricane/Twister/Typhoon-era engine surface | `Storm::engine_type`, exactly `std::mt19937_64` |
| Implicit engine ownership | `Storm::Generator`, injected engines, or explicit TLS convenience |
| Seed zero requests entropy | Seed zero is deterministic |
| Entropy described as hardware randomness | Explicit, potentially throwing entropy reseed with no hardware guarantee |
| Broad `Get*` and helper namespaces | Small integer/index/range/dice/canonical core |
| Storm-owned distribution and positional recipes | Standard distributions or application-owned algorithms |
| Signed sentinel dice/index conventions | Nonnegative size-based APIs |
| Order-independent ranges | Directed, Python-like ranges |
| Non-commercial Creative Commons | MIT |

## Mechanical include, version, and engine migration

| Removed Storm 4 name | Storm 5 migration |
| --- | --- |
| `#include <Storm.hpp>` | `#include <Storm/Storm.hpp>` |
| `get_version()` | Read `Storm::version` |
| `Float`, `Integer`, `Unsigned` | Use `double`, `std::int64_t`, and `std::uint64_t` directly |
| `Engine::Hurricane`, `Engine::Twister`, `Engine::Typhoon` and related aliases | Use `Storm::engine_type` or own a `Storm::Generator` |
| `Engine::seed(value)` | Use `generator.seed(value)` for owned state or `Storm::seed(value)` for current-thread TLS |
| Hurricane shuffle/drop behavior | Intentionally removed; do not emulate unless an application protocol requires that historical stream |

Storm 5 algorithms accept `Storm::engine_type&`. Prefer one owned generator per
logical stream:

```cpp
#include <Storm/Storm.hpp>

Storm::Generator generator{1234};
const auto value = Storm::uniform_integer(generator.engine(), -10, 10);
```

Copying an engine forks its deterministic state. `Storm::seed(value)` changes
only the calling thread's convenience engine; it never changes owned engines
or another thread's TLS state.

`Storm::reseed_from_entropy()` and `Generator::reseed_from_entropy()` collect
multiple `std::random_device` words and pass them through `std::seed_seq`. They
may throw. `std::random_device` is not promised hardware-backed,
nondeterministic, or cryptographically secure.

## Removed operations with direct Storm 5 mappings

These migrations keep the operation in the clean Storm core but may require
adapting old argument conventions.

| Removed Storm 4 call/family | Storm 5 operation | Required adaptation |
| --- | --- | --- |
| `GetFloat::canonical_variate()` | `Storm::canonical(engine)` | Inject the engine; result is canonical `[0,1)` |
| `GetInt::uniform_int_variate(a,b)` | `Storm::uniform_integer(engine, low, high)` | Order bounds explicitly; reversed bounds now throw |
| `GetInt::uniform_uint_variate(a,b)` | `Storm::uniform_unsigned(engine, low, high)` | Order bounds explicitly |
| Positive `GetInt::random_below(n)` | `Storm::uniform_index(engine, size)` | Use a size and convert the result to the application's integer type safely |
| Positive `GetIndex::random_index(n)` | `Storm::uniform_index(engine, size)` | Zero is invalid; signed negative-index behavior is gone |
| `GetInt::random_range(start,stop,step)` | `Storm::random_range(engine,start,stop,step)` | Rewrite callers for directed phase; bounds are never reordered |
| Positive `GetInt::d(sides)` | `Storm::roll_die(engine,sides)` | Use a positive size; zero and negative sides are invalid |
| Positive `GetInt::dice(rolls,sides)` | `Storm::roll_dice(engine,rolls,sides)` | Use nonnegative counts and positive sizes |
| `GetInt::ability_dice(count)` | `Storm::ability_dice(engine,count)` | Count must be at least 3; no legacy clamp to `[3,9]` |

Negative `random_below`, signed dice, signed ZeroCool indices, and the old
order-independent range phase have no direct compatibility mode. Preserve
them at the application layer only when they are intentional product behavior,
using checked arithmetic and explicit tests.

## Removed operations that map to `<random>`

Storm 5 does not wrap standard-library distributions. Validate application
inputs, construct the distribution, and invoke it with an injected engine.
These distributions may not produce identical sample streams across standard
libraries or toolchain versions.

| Removed Storm 4 family | Standard-library starting point |
| --- | --- |
| `GetBool::bernoulli_variate`, `percent_true` | `std::bernoulli_distribution` after validating and scaling probability |
| `GetFloat::uniform_real_variate` | `std::uniform_real_distribution<double>` |
| `GetFloat::exponential_variate` | `std::exponential_distribution<double>` |
| `GetFloat::gamma_variate` | `std::gamma_distribution<double>` |
| `GetFloat::weibull_variate` | `std::weibull_distribution<double>` |
| `GetFloat::normal_variate` | `std::normal_distribution<double>` |
| `GetFloat::log_normal_variate` | `std::lognormal_distribution<double>` |
| `GetFloat::extreme_value_variate` | `std::extreme_value_distribution<double>` |
| `GetFloat::chi_squared_variate` | `std::chi_squared_distribution<double>` |
| `GetFloat::cauchy_variate` | `std::cauchy_distribution<double>` |
| `GetFloat::fisher_f_variate` | `std::fisher_f_distribution<double>` |
| `GetFloat::student_t_variate` | `std::student_t_distribution<double>` |
| `GetInt::binomial_variate` | `std::binomial_distribution` |
| `GetInt::negative_binomial_variate` | `std::negative_binomial_distribution` |
| `GetInt::geometric_variate` | `std::geometric_distribution` |
| `GetInt::poisson_variate` | `std::poisson_distribution` |

Example:

```cpp
Storm::Generator generator{42};
std::binomial_distribution<std::int64_t> binomial{10, 0.3};
const auto successes = binomial(generator.engine());
```

Check finiteness as well as the mathematical domain before constructing a
floating-parameter distribution. Translate resulting C++ exceptions at any
foreign-language boundary.

## Intentionally cut recipes and helpers

These removed families are not part of the Storm 5 random core and do not have
a blessed replacement:

- `GetFloat::beta_variate`, `pareto_variate`, `vonmises_variate`, and
  `triangular_variate`
- `GetInt::plus_or_minus`, `plus_or_minus_linear`, and `plus_or_minus_gauss`
- `GetIndex::front_*`, `middle_*`, `back_*`, `quantum_*`, and `quantum_monty`,
  including the custom gauss, poisson, linear, quantum, and Monty profiles
- signed/negative dice, negative `random_below`, and signed ZeroCool behavior
- `GearBox::clamp`, `approximation_clamp`, and `analytic_continuation`

The old distribution-like and positional names described historical custom
recipes, not canonical mathematical distributions. Reimplement a recipe at
the application layer only when a current product requirement justifies it.
Give the replacement a precise name, accept an injected engine, document its
support and error contract, and add deterministic domain/statistical tests.

For removed `Meters` accessors, use standard facilities directly:

| Removed helper | Replacement |
| --- | --- |
| `Meters::max_uint`, `min_int`, `max_int`, `min_float`, `max_float` | `std::numeric_limits<T>` |
| `Meters::min_below`, `min_above` | `std::nextafter` with an explicit direction |
| `GearBox::clamp` | `std::clamp` after establishing ordered bounds |

Application callbacks formerly routed through other `GearBox` helpers should
be ordinary application functions with checked arithmetic and an explicit
error contract.

## Behavior and test changes

Adopters must update code, docs, and tests for all of these differences:

- `seed(0)` is deterministic; entropy reseeding is explicit and may throw.
- TLS seeding affects the current thread only.
- The true minimum signed value is `std::numeric_limits<long long>::min()`, not
  `-max()`.
- Index size zero is an error, never a valid sentinel selection.
- Zero-trial binomial sampling returns zero for valid probability when an
  application uses the standard distribution.
- Invalid domains, NaN, and infinity must be rejected before constructing
  distributions.
- Integer operations support `LLONG_MIN`, `LLONG_MAX`, the complete signed
  interval, and values around `2^53` without conversion through `double`.
- `canonical` returns `[0,1)`.
- Dice counts/sides and indices are size-based. Overflow and invalid domains
  are errors.
- `random_range` is directed; old order-independent phase semantics must be
  migrated deliberately.
- Ability dice validates before conversion, does not allocate proportional to
  its count, and does not apply the v4 `[3,9]` clamp.
- Real `std::thread` tests are required for TLS isolation. Multiprocessing
  tests do not exercise C++ thread-local state.

Do not use exact standard-distribution samples as cross-toolchain golden
vectors. Storm promises exact major-v5 stability only for its own bounded
integer/index algorithms with the same engine state and arguments.
