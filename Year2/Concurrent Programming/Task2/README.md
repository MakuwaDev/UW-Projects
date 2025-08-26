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

  α(**d**, **∅**, **{1}**)=**(d−1)$$^2$$**.

These can be proven optimal, but the project requires computational verification for as large 
**d** as possible, and also for other forced multisets.

## Implementation Task
- A **reference solution** (sequential recursive implementation using bitsets) is provided.
- You must implement two alternative versions of the same algorithm:
  - **Non-recursive** (single-threaded) implementation (e.g. using an explicit stack).
  - **Parallel** (multi-threaded) implementation using `pthreads`, aiming for the best scalability.
