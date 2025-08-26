# Binder - A Copy-on-Write Notebook Container

## ⚠️ Disclaimer
This was a pair project, co-author: **Aleks Lukosek**.

## Short task summary/translation:

This project implements a `binder` class template, a structured notebook system designed for organizing and sharing study notes efficiently. It supports **copy-on-write (COW)** semantics, ensuring memory efficiency while allowing modifications without affecting shared copies.

## Class Template Definition

```cpp
  namespace cxx {
  template <typename K, typename V> class binder;
}
```
- `K` (key) represents a unique **bookmark** for each note and must support **copying, moving, assignment, and ordering.**
- `V` (value) stores the **note content** and must have a **public copy constructor and destructor.**

## Features and Operations

### Constructors & Assignment

- **Default constructor**: Creates an empty binder.
- **Copy & move constructors**: O(1) complexity.
- **Copy-and-swap assignment**: O(1) complexity + destruction time of overwritten object.

### Note Management

- `insert_front(K const&, V const&)` – Inserts a note at the front. Throws `std::invalid_argument` if the bookmark already exists. *(O(log n))*
- `insert_after(K const&, K const&, V const&)` – Inserts a note after a given bookmark. Throws if `prev_k` is not found. *(O(log n))*
- `remove()` – Removes the first note. Throws if empty. *(O(log n))*
- `remove(K const&)` – Removes a specific note by its bookmark. Throws if not found. *(O(log n))*
- `read(K const&)` / `read(K const&) const` – Retrieves a reference to a note by its bookmark. *(O(log n))*
- `size() const` – Returns the number of notes. *(O(1))*
- `clear()` – Removes all notes. *(O(n))*

### Iteration Support

Implements const_iterator with:
- `cbegin()`, `cend()`
- Standard iterator operations (`=`, `==`, `!=`, `++`, `*`, `->`)
- Forward iteration in *O(1)* time.

Safe as long as the binder isn't copied.


## Additional Requirements
- **Exception Safety**: Strong exception guarantee for all operations.
- **Memory Efficiency**: Uses **copy-on-write (COW)** to share resources until modifications are made.
- **Compilation**: Must compile with `g++ -Wall -Wextra -O2 -std=c++20`.
