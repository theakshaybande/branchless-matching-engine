# Branchless Matching Engine

This repository demonstrates a compact matching-engine core in modern C++20, comparing a traditional branching implementation against a branchless-style implementation for low-latency and HFT-oriented learning. The goal is not to model a full exchange, but to show how a hot matching path can be written with fixed data structures, predictable updates, and disciplined benchmarking.

## Why this matters

In latency-sensitive systems, small control-flow decisions can matter. Branch predictors do a good job when data is regular, but unpredictable order flow can make mispredictions more expensive than a few arithmetic operations. HFT engineers care about these tradeoffs because matching logic often sits on a performance-critical path where predictable execution, cache-friendly layouts, and careful benchmarking are part of normal engineering discipline.

## What's inside

- A compact aligned `Order` model and `MatchResult`
- A straightforward branching matcher using `if`
- A branchless-style matcher using boolean-to-integer masking
- A deterministic synthetic benchmark over 1,000,000 order pairs
- Lightweight assertion-based tests for correctness and equivalence

## Matching rule

- A trade happens when `buy.price >= sell.price`
- Filled quantity is `min(buy.quantity, sell.quantity)`
- Execution price is `sell.price`

If prices do not cross, quantities remain unchanged.

## Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Run

```bash
./matching_demo
./matching_benchmark
./matching_tests
```

On Windows with a Visual Studio generator, the executables will typically live under the chosen build configuration directory, for example `Debug` or `Release`.

## Example output

### Demo

```text
Branchless Matching Engine Demo
===============================

Branching match
  matched: true
  traded quantity: 250
  execution price: 100.00
  remaining buy: 150
  remaining sell: 0

Branchless match
  matched: true
  traded quantity: 250
  execution price: 100.00
  remaining buy: 150
  remaining sell: 0

No-cross example
  matched: false
  traded quantity: 0
  execution price: 0.00
  remaining buy: 500
  remaining sell: 500
```

### Benchmark

```text
Branching vs Branchless Matching Benchmark
=========================================
order pairs: 1000000
runs: 5 (best runtime shown)

branching        runtime: 18077300  ns avg: 18.08    ns/match checksum: 166774611
branchless       runtime: 20063400  ns avg: 20.06    ns/match checksum: 166774611

Note: microbenchmarks are sensitive to CPU model, compiler, optimization level,
and the fraction of orders that actually cross.
```

### Tests

```text
All matching engine tests passed.
```

## Notes on benchmarking

Branchless code is not automatically faster. Actual performance depends on the CPU microarchitecture, compiler, optimization level, inlining decisions, branch predictability, and the statistical shape of the input stream. The benchmark in this repository is designed to be clean and repeatable, but it is still a microbenchmark and should be interpreted accordingly.

## Future extensions

- Vectorized batch matching over packets of synthetic orders
- Cache-aware order book containers with fixed-capacity levels
- Integer tick-based price representation instead of `double`
- Lower-level latency instrumentation with `rdtsc`
- Integration into a toy limit-order-book simulator
