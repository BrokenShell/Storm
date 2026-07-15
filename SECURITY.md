# Security policy

## Supported versions

| Version | Security updates |
| --- | --- |
| 5.x | Supported |
| 4.x and earlier | Not supported |

Fortuna's current vendored Storm 4.0.4 header is isolated from the Storm 5
release. That compatibility fact does not make Storm 4 a supported security
branch.

## Reporting a vulnerability

Report a suspected vulnerability privately through the repository's GitHub
Security Advisory interface. Include the affected version, platform/compiler,
minimal reproducer, impact, and any proposed remediation. Do not open a public
issue for an undisclosed vulnerability.

If private reporting is unavailable, contact the repository owner through the
contact route shown on the GitHub repository and request a private channel
without including exploit details in the first message.

Maintainers should acknowledge a report, reproduce and assess it, prepare a
coordinated fix, and credit the reporter when requested and appropriate. No
response or remediation deadline is promised by this policy.

## Randomness is not cryptography

Storm uses `std::mt19937_64`, a deterministic simulation engine. Its state can
be reconstructed from sufficient output, and a known seed reproduces its
stream. Storm is not suitable for:

- cryptographic keys, salts, initialization vectors, or nonces
- passwords, reset links, session identifiers, or authentication tokens
- security boundaries or adversarially observable draws
- any protocol that requires a cryptographically secure random-number
  generator

`Storm::reseed_from_entropy()` uses multiple `std::random_device` values via
`std::seed_seq`, but the C++ standard does not guarantee that `random_device`
is hardware-backed, nondeterministic, or cryptographically secure. Entropy
reseed does not turn `std::mt19937_64` into a cryptographic generator.

For security-sensitive randomness, use the operating system's documented CSPRNG
or a reviewed cryptographic library appropriate to the target platform.
