# Garden Growth and Analysis  
## Short task summary/translation:
The government of Bajtocja is funding the creation of a **grand garden**. Professor Makary oversees the project, designing a **long alley** divided into **n segments**, each initially containing trees of height **1**.  

Professor Makary performs two types of operations:  
1. **Fertilization (`N aᵢ bᵢ kᵢ`)** – Increases the height of trees in segments **[aᵢ, bᵢ]** by **kᵢ**.  
2. **Query (`C aᵢ bᵢ`)** – Determines the length of the **longest contiguous non-decreasing sequence** of tree heights within segments **[aᵢ, bᵢ]**.  

## Input  
- Two integers **n, m** (`1 ≤ n, m ≤ 500 000`) – Number of segments and operations.  
- **m lines** describing operations:  
  - `"N aᵢ bᵢ kᵢ"` (`1 ≤ aᵢ ≤ bᵢ ≤ n`, `1 ≤ kᵢ ≤ 1000`) – Fertilization.  
  - `"C aᵢ bᵢ"` (`1 ≤ aᵢ ≤ bᵢ ≤ n`) – Query for the longest non-decreasing subsequence.  

## Output  
For each **`C` query**, print a single integer – the length of the longest **contiguous** non-decreasing sequence in the specified range.  
