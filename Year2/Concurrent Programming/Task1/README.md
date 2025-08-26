# Concurrent Circuits

The task is to implement a solver for concurrent evaluation of boolean circuits represented as trees of nodes.  
A circuit consists of constants (`true`, `false`), logical operators (`AND`, `OR`, `NOT`), conditional (`IF`), and threshold operators (`GT`, `LT`).  
The solver must support evaluating multiple circuits in parallel and compute values of subexpressions concurrently, respecting interruption and stop signals.  
