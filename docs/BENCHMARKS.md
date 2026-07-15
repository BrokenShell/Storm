# Benchmark methodology

Storm benchmarks are an opt-in engineering tool. They are separate from CTest
correctness tests and are not a hosted pass/fail timing gate.

## Purpose

Benchmarks may answer bounded questions such as:

- What overhead does Storm's bounded mapping add relative to an equivalent
  standard-library baseline?
- How do injected-engine and TLS call paths compare in the same build?
- Does a proposed implementation materially regress a named workload on the
  same machine and toolchain?

They must not be used to claim that Storm is secure, has better entropy, or is
universally faster than another library.

## Build

Enable the benchmark target explicitly in a Release build using the CMake
benchmark option exposed by the project. Keep tests enabled when practical so
correctness is established before timing.

Do not use `-ffast-math`, `-march=native`, link-time optimization, or another
non-default optimization silently. Such flags may be used for an explicitly
labeled experiment, but both Storm and its baseline must use the same settings
and the report must disclose them.

## Workloads and baselines

Every measured Storm operation must have either:

- an equivalent standard-library baseline using the same `std::mt19937_64`
  state and comparable output domain, or
- an explanation that no equivalent standard operation exists.

Examples include raw engine calls, uniform bounded integers, uniform indices,
canonical doubles, and dice loops. Setup, engine construction, entropy
acquisition, and sampling are different costs and must not be mixed unless the
workload explicitly intends to measure all of them.

Use fixed deterministic seeds for timed sampling. Entropy reseeding is an I/O
and platform facility; benchmark it separately if it is relevant.

## Result consumption

The benchmark executable must consume each generated result in an observable
way, such as combining it into a checksum reported at the end. Discarded
results permit the optimizer to remove the work and invalidate the timing.

The consumption operation must be equivalent across compared workloads and
must not dominate only one side of the comparison. Report the final checksums
to make accidental optimization or setup drift visible; matching checksums are
not required when algorithms intentionally map bits differently.

## Measurement procedure

1. Build Storm and all baselines together with the same compiler, language
   mode, optimization level, architecture flags, and sanitizer state.
2. Run correctness tests first.
3. Warm up the benchmark path before recording samples.
4. Use enough iterations for the timed region to dominate clock resolution.
5. Record multiple samples and report the median plus a spread measure such as
   minimum/maximum or percentiles.
6. Alternate or randomize workload order when temperature and frequency scaling
   could bias a fixed order.
7. Keep machine load stable and disclose unavoidable background activity.
8. Preserve raw output with the summary.

Do not report more precision than the measurement supports. A single run is an
observation, not a general performance conclusion.

## Required report metadata

Record:

- Storm commit and version
- compiler identity and full version
- standard library and version where available
- operating system and architecture
- CPU model and relevant power/frequency mode
- build type and all non-default compiler/linker flags
- workload parameters, iterations, warmups, and sample count
- deterministic seed
- whether TLS, injected engines, allocation, or construction is inside the
  timed region
- sanitizer/instrumentation state
- raw results, summary statistic, and result checksum

## Interpreting results

Exact output stability for Storm-owned bounded integer/index algorithms within
v5 does not imply equal performance across machines. Conversely, the standard
library may produce a different valid distribution stream across toolchains,
so output sequence differences do not by themselves indicate a performance or
correctness regression.

Only claim a regression or improvement when the compared workloads are
equivalent, the difference is larger than observed run-to-run variation, and
the environment is controlled and disclosed.

CI should build and optionally smoke-run the benchmark target to catch bit rot,
but hosted runner timing must not gate a pull request.
