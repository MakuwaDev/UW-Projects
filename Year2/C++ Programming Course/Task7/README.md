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
