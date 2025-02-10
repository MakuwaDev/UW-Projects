# Efficient Querying of a Number Sequence
## Short task summary/translation:  
Implement a data structure that allows efficient querying of a sequence of numbers. Furthermore, the elements of the sequence can be revealed online, meaning that new elements may be added dynamically.

For a given sequence X=x0​,x1​,...,xn−1​, we are interested in the following function:
## Function: prevInRange(i, [lo, hi])

  `prevInRange(i, [lo, hi]) = max{0 ≤ j ≤ i : xj ∈ [lo, hi]} or -1 (if such an index does not exist).`

## Required Functions:

    void init(const vector<int> &x)
    Initializes the sequence X with the values from the vector x. The sequence can contain any integer values.

    int prevInRange(int i, int lo, int hi)
    Calculates the value of prevInRange(i, [lo, hi]).  
    You can assume that 0 ≤ i <∣X∣ and INT_MIN ≤ lo ≤ hi ≤ INT_MAX.

    void pushBack(int v)
    Adds an element with value v to the end of the sequence.

    void done()
    Frees all the memory used to handle the sequence X.

## Requirements:

    The time complexity of the prevInRange and pushBack functions must be O(log⁡z),  
    where z is the range of integer values.
    The time complexity of the init function should be O(∣X∣log⁡z) at worst,  
    where |X| is the size of the initial sequence.
    Avoid any solution with brute force, as it will result in a score of 0.
    The program should be free of memory leaks.

## Example:

### Input:
```text
init([5, 10, 3, 8, 12]);
prevInRange(4, 5, 10); // Returns: 3
pushBack(7);
prevInRange(5, 5, 10); // Returns: 4
done();
