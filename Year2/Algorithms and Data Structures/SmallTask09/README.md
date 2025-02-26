# Finding Clearings in a Bit Park  
## Short task summary/translation:
In **Bit Park**, there are **n** clearings numbered from **1 to n**. Some pairs of clearings are connected by **bidirectional paths**, forming a **binary tree** rooted at clearing **1**.  

Bajtek and Bajtyna are playing a game:  
1. One child picks a clearing **a** and a non-negative integer **d**.  
2. The other must find **any clearing** exactly **d** distance away from **a**.  
3. If no such clearing exists, they must declare it.  

Bajtek wants to **automate** this process, so your task is to write a program that finds these clearings.  

## Input  
- **n** (`2 ≤ n ≤ 500 000`) – Number of clearings.  
- **n** lines, each containing **two integers** `aᵢ` and `bᵢ`:  
  - They describe the two child clearings connected to clearing **i**.  
  - A value of `-1` means no path exists in that direction.  
- The structure always forms a **valid rooted tree** with root **1**.  
- **m** (`1 ≤ m ≤ 500 000`) – Number of queries.  
- **m** lines, each containing two integers **a** and **d** (`1 ≤ a ≤ n, 0 ≤ d < n`).  

## Output  
- For each query, output **any valid clearing** at distance **d** from **a**.  
- If no such clearing exists, output `-1`.
