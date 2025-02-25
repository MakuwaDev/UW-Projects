# Cellular Sorting Reversal  
## Short task summary/translation:

Cellular sorting is an unusual sorting algorithm with a relatively high time complexity. It operates step by step, repeatedly applying a transformation until the sequence becomes non-decreasing.  

In each step, the algorithm scans the sequence from left to right and constructs a new sequence:  
- The first element is placed in the result.  
- Each subsequent element is inserted at the **beginning** if it is smaller than the previous one, or at the **end** if it is larger.  

For example, applying one step to `[5, 6, 2, 1, 4, 3]` produces:  
1. `[5]`  
2. `[5, 6]`  
3. `[2, 5, 6]`  
4. `[1, 2, 5, 6]`  
5. `[1, 2, 5, 6, 4]`  
6. `[3, 1, 2, 5, 6, 4]`  

Given a sequence, the task is to determine how many different sequences could transform into it in **one step** of cellular sorting.  

## Input  
- An integer **n** (1 ≤ n ≤ 1000), the length of the sequence.  
- A permutation of `{1, ..., n}` representing the target sequence.  

## Output  
- The number of sequences that could transform into the given sequence in one step, modulo **10⁹**.  
