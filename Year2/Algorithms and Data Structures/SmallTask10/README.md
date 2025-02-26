# Lexicographical Substring Comparison  

Given a string **s** of length **n** consisting of lowercase English letters, we analyze its **substrings**, which are contiguous segments of **s** in the form **s[i..j]**. The goal is to **lexicographically compare** pairs of such substrings.  

A substring **u** is **lexicographically smaller** than **v** (`u < v`) if:  
1. **u is a proper prefix** of **v** (i.e., u is a starting segment of v but shorter than v), or  
2. **u and v differ at some position**, and the first differing character in **u** is smaller than in **v**.  

## Input  
- **n, m** (`1 ≤ n, m ≤ 300 000`) – Length of string **s** and number of queries.  
- **s** – A string of length **n**.  
- **m queries**, each containing four integers **a, b, c, d** (`1 ≤ a ≤ b ≤ n`, `1 ≤ c ≤ d ≤ n`), asking whether substring **s[a..b]** is smaller, greater, or equal to **s[c..d]**.  

## Output  
- **m lines**, each containing:  
  - `'<'` if **s[a..b] < s[c..d]**  
  - `'>'` if **s[a..b] > s[c..d]**  
  - `'='` if **s[a..b] = s[c..d]**  
