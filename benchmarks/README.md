<!-- SPDX-License-Identifier: MIT -->
# Storm benchmark methodology

The benchmark is an opt-in diagnostic executable, not a correctness test or a
hosted timing gate. Configure a Release build with
`-DSTORM_BUILD_BENCHMARKS=ON`, build `storm_benchmark`, and run it on an idle
machine. An optional positive integer argument selects the number of draws.

The executable compares two equivalent operations using the same fixed seed:

- `Storm::uniform_index` against `std::uniform_int_distribution<std::size_t>`
- `Storm::canonical` against `std::generate_canonical<double, ...>`

Each workload performs an untimed warmup first. Every warmup and measured result
contributes to a checksum that is printed, preventing the optimizer from
discarding the work. Each implementation owns a separate `std::mt19937_64`
engine. The output reports nanoseconds per draw and the toolchain-dependent
checksum; it does not assert that one implementation must be faster.

For useful comparisons, record the CPU, operating system, compiler and version,
standard library, optimization flags, iteration count, and several repeated
runs. Compare medians only within the same environment. Do not compare Debug or
sanitized builds, and do not infer randomness quality or security from timing.
