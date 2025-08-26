# Counters

## ⚠️ Disclaimer
This was a pair project, co-author: **Gracjan Barski**.


## Modules
This project introduces **C++20 modules** through an object-oriented implementation of various counter types.  
The goal is to learn:
- Basics of **C++ modules**.
- **Object-oriented programming** in C++.

## Counter Types

- **Modulo Counter** – Counts from 0 to *M*, then wraps to 0. Triggers an overflow event when rolling over.
- **Fibonacci Counter** – Counts up to `UINT64_MAX`, triggering events when the value is a Fibonacci number.
- **Geometric-Decimal Counter** – Cycles through ranges (0–9, 0–99, 0–999, …) and triggers an overflow event when rolling over.

All counters:
- Have a prescaler `<P>` that skips `<P>` pulses after each counted pulse.
- Store integer values in the range `[0, UINT64_MAX]`.

## Program Specification

The program manages multiple counters, supporting commands to:
- Create counters (`M`, `F`, `G` commands).
- Delete counters (`D`).
- Print counter values (`P`).
- Send pulses to all counters and list triggered events (`A`).

Invalid operations (e.g., creating an existing counter or deleting a non-existent one) produce an error message  
`ERROR L` to standard error, where `L` is the input line number.

## Requirements

- Implemented using **OOP** and **C++20 modules** (`.cppm` files).
- No preprocessor directives (`#`).
- Delivered with a `Makefile` supporting incremental builds, `.PHONY`, `clean`, and producing an executable `counters`.
