# Counting k-Inversions  
## Short task summary/translation:
Given a permutation of numbers `{1, ..., n}`, a **k-inversion** is a sequence of indices `i₁, i₂, ..., iₖ` such that:  
- `1 ≤ i₁ < i₂ < ... < iₖ ≤ n`  
- The values at these indices form a strictly decreasing sequence: `a[i₁] > a[i₂] > ... > a[iₖ]`.  

The task is to determine the number of k-inversions in the given permutation.  

## Input  
- Two integers **n** and **k** (`1 ≤ n ≤ 20 000`, `2 ≤ k ≤ 10`).  
- A permutation of `{1, ..., n}`.  

## Output  
- The number of k-inversions, modulo **10⁹**.  
