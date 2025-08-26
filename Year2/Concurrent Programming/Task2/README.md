# Computational Verification of a Combinatorial Hypothesis

## Short task summary/translation:

We are interested in pairs of *multisets* of natural numbers with certain properties.
- For a multiset **A**, we denote **∑A** as the sum of its elements.
- Two multisets **A**,**B** are called `uncontestable` if:
  - **∑A**=**∑B**,
  - and the only subset pairs **A′**⊆**A**,**B′**⊆**B** with equal sums are the trivial ones: both empty, or both equal to the full sets.
- For a fixed parameter **d**≥**3**, multisets must be **d-bounded** (i.e., all elements between **1** and **d**).

The computational problem stands as follows:

Given d, and initial multisets **A0**, **B0**, find **d-bounded** `uncontestable` supersets **A**⊇**A0**, **B**⊇**B0** maximizing **∑A**=**∑B**.
This maximum value is denoted `α(**d**, **A0**, **B0**)`.
If no such sets exist, then α=**0**.

## Examples

- For **A0**=**B0**=**∅**:

  α(**d**, **∅**, **∅**)=**d(d−1)**.

- For **A0**=**∅**, **B0**=**{1}**:

  α(**d**, **∅**, **{1}**)=**(d−1)^2**.

These can be proven optimal, but the project requires computational verification for as large 
**d** as possible, and also for other forced multisets.

## Implementation Task
- A **reference solution** (sequential recursive implementation using bitsets) is provided.
- You must implement two alternative versions of the same algorithm:
  - **Non-recursive** (single-threaded) implementation (e.g. using an explicit stack).
  - **Parallel** (multi-threaded) implementation using `pthreads`, aiming for the best scalability.


Key points:

- The algorithm explores all branches of the recursion tree, incrementally building **A**, **B**.
- To optimize, sumsets (**ΣA**, **BΣ**) are represented using efficient bitsets.
- You are not allowed to change how sumsets work; the challenge is to restructure for parallelism, not micro-optimize bit operations.
- Each thread should mostly explore its own branch, occasionally sharing work to balance load.

## Scalability & Report

- Define scalability factor as **ts**/**tn**, where:
  - ts = runtime of the reference implementation (1 thread).
  - tn = runtime of the parallel implementation with n threads.
  - Ideally, scalability = n.

- Report requirements:
  - PDF document with plots showing scalability for **d**∈{**5**, **10**, **15**, **20**, **25**, **30**, **32**, **34**}, with **A0**=**∅**, **B0**=**{1}**.
  - Use number of threads = powers of **2** (**1**, **2**, **4**, **8**, … up to at least **8**, max **64**).
  - If runtime > **1 min** → stop and mark scalability as **−1**.
 
## Input/Output Format

### Input:
```
t d n m
[A0 elements...]
[B0 elements...]
```
