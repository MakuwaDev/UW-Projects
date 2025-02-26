# Maximum Domino Placement Value  

Given a **k × n** rectangular grid where each cell contains an integer, the task is to place **1×2 dominoes** on the board such that:  
- Each domino covers exactly **two adjacent cells**.  
- No cell is covered by more than **one** domino.  

The **value of a placement** is the sum of the numbers in the covered cells. The goal is to determine the **maximum possible value** of such a placement.  

## Input  
- Two integers **n, k** (`1 ≤ n ≤ 1000`, `1 ≤ k ≤ 12`) – Grid dimensions.  
- **k rows**, each containing **n integers** (`-10⁶ ≤ aᵢⱼ ≤ 10⁶`) representing the grid values. The **i-th row** contains values for row **i**.  

## Output  
- A single integer: the **maximum possible sum** of covered values.  
