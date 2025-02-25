# Dynamic Sequence Operations  

This program implements a **dynamic sequence** data structure that supports two operations:  

- **Insert**: `insert(j, x, k)` – Inserts **k** copies of element **x** before the **j-th** position, shifting subsequent elements to the right.  
- **Get**: `get(j)` – Returns the value at the **j-th** position in the sequence.  

## Input  
- An integer **m** (`1 ≤ m ≤ 500 000`): the number of operations.  
- **m** lines, each containing:  
  - `'i' j' x k'` for an **insert** operation.  
  - `'g' j'` for a **get** operation.  

### Special Conditions  
- The index **j** is computed as:  
  - `j = (j' + w) mod (n + 1)` for **insert**  
  - `j = (j' + w) mod n` for **get**  
- **w** is the result of the last **get** operation (or `0` initially).  

## Output  
- For each **get** operation, print the retrieved element.  
