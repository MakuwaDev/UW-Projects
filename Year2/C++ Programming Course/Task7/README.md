# Functional Lists

This task focuses on **functional programming in C++** using **lambdas** and `std::function`.  
The goal is to implement immutable lists that store elements implicitly and can only be processed by applying a function with an accumulator.

## Learning Objectives
- Lambda expression syntax
- `std::function` type
- Basics of functional programming in C++

## List Concept
A list type `L` must provide:
```cpp
template <typename F, typename A> A operator()(F f, A a);
```
where type `F` has a template method of signature
```cpp
template <typename X> A operator()(X x, A a);
```
Processing a list `l` with function `f` and accumulator `a` must satisfy:
```
l(f, a) = f(x0, f(x1, ... f(xn-1, a) ...))
```
where `x0, x1, ...` are the elements of the list.


## Required Operations (in namespace `flist`)

- `auto empty` – empty list.

- `auto cons(x, l)` – prepend x to list l.

- `auto create(...)` – create list from given arguments.

- `auto of_range(r)` – create list from range r.

- `auto concat(l, k)` – concatenate two lists.

- `auto rev(l)` – reverse a list.

- `auto map(m, l)` – apply transformation m(x) to each element.

- `auto filter(p, l)` – keep only elements satisfying p(x).

- `auto flatten(l)` – flatten a list of lists.

- `std::string as_string(l)` – string representation of list.

## Constraints

- All list-creating operations take arguments by value (copies). Use `std::ref` to pass by reference.

- Running `l(f, a)` assumes `f` returns the same type as `a`.

- Prefer lambdas over custom structs where possible.

- Helper definitions should be hidden in `flist::detail`.

## Compilation
Compiled using
```
/opt/llvm/19.1.4/bin/clang++
```
with the following command:
```bash
clang++ -Wall -Wextra -std=c++23 -O2 *.cpp
```
