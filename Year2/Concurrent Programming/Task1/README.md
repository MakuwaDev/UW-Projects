# Concurrent Circuits

## Short task summary/translation:
The task is to implement a program for concurrent evaluation of boolean expressions represented as circuits.  
A circuit is given as a tree structure composed of nodes representing constants (`true`, `false`), negation (`NOT`), conjunction (`AND`), disjunction (`OR`), conditionals (`IF`), and threshold operators (`GT`, `LT`).  
The semantics follow standard boolean logic with extensions for thresholds (e.g. `GTx` is true if at least `x+1` arguments are true).

## Requirements
- Circuits are represented by the class `Circuit` with a root node of type `CircuitNode`.  
- A solver implements the interface:
  ```java
  public interface CircuitSolver {
      public CircuitValue solve(Circuit c);
      public void stop();
  }
  ```
  where `solve` immediately returns a `CircuitValue` object, and the actual value is obtained by calling `getValue()`, which may block until the computation finishes.
- The solver must:
  - Allow concurrent evaluation of multiple circuits.
  - Evaluate subexpressions concurrently (e.g., arguments of `AND`, `OR`, etc.).
  - Support interruption: after calling `stop()`, all ongoing computations must terminate, and new tasks must be rejected. Interrupted evaluations may throw `InterruptedException` on `getValue()`.

## Constraints
- Each circuit has a unique tree of nodes; different `solve()` calls receive disjoint circuits.
- Evaluation of leaf nodes (`LeafNode.getValue()`) and argument retrieval (`getArgs()`) may take arbitrary time, but are free of side effects and handle interruptions correctly.
- Implementations must not use `CompletableFuture<T>` or its derivatives.
