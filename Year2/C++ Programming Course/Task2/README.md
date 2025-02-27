# String Queue (strqueue)

## Short task summary/translation:
This project involves implementing a **string queue module** in C++ that is compatible with C. The module consists of a **header file (`strqueue.h`)** and an **implementation file (`strqueue.cpp`)**. The goal is to practice using STL containers, integrating C++ with legacy C code, and handling global object initialization.

## Functionality
The module provides the following operations:

- **Queue creation & deletion**  
  - `strqueue_new()` → Creates a new empty queue and returns its ID.  
  - `strqueue_delete(id)` → Deletes the queue with the given ID.  

- **Queue operations**  
  - `strqueue_size(id)` → Returns the number of elements in the queue.  
  - `strqueue_insert_at(id, position, str)` → Inserts `str` at `position` (or appends if out of range).  
  - `strqueue_remove_at(id, position)` → Removes the string at `position`.  
  - `strqueue_get_at(id, position)` → Returns a pointer to the string at `position` (or `NULL` if out of range).  
  - `strqueue_clear(id)` → Removes all elements from the queue.  

- **Queue comparison**  
  - `strqueue_comp(id1, id2)` → Lexicographically compares two queues, returning:
    - `-1` if `queue1 < queue2`
    - `0` if `queue1 == queue2`
    - `1` if `queue1 > queue2`

## Implementation Details
- Uses **STL containers** (e.g., `unordered_map`, `deque`) instead of custom structures.  
- The queue **stores copies of strings**, not just pointers.  
- Supports both **C and C++**:
  - In **C++**, functions are in the `cxx` namespace.
- Global variables and helper functions are **hidden from external access**.

## Diagnostics & Debugging
- Functions print **diagnostic messages** with call parameters and results.  
- **Assertions** help verify data consistency.  
- Compiling with `-DNDEBUG` disables debugging output and assertions.

## Compilation
The solution will be compiled on the **students** machine using:  
  ```sh
  g++ -Wall -Wextra -O2 -std=c++20 strqueue.cpp -o strqueue
  ```
