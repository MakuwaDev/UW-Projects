# NAND Gates Library  

## Short task summary/tranlsation:
The goal is to implement a dynamically loaded C library for simulating combinational circuits built from NAND gates. Each gate has a non-negative number of inputs and a single output. The NAND operation follows standard logic rules:  
- A gate with no inputs always outputs `false`.  
- A single-input gate acts as a NOT gate.  
- An `n`-input gate outputs `false` only if all inputs are `true`; otherwise, it outputs `true`.  
- A gate’s output can connect to multiple inputs of other gates, but each input must have exactly one source.  

## Library Interface  
The provided `nand.h` header defines the required API, including:  
- `nand_t *nand_new(unsigned n);` – Creates an `n`-input NAND gate.  
- `void nand_delete(nand_t *g);` – Destroys a gate and frees memory.  
- `int nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k);` – Connects `g_out`'s output to `g_in`'s `k`-th input.  
- `int nand_connect_signal(bool const *s, nand_t *g, unsigned k);` – Connects a boolean signal to a gate’s input.  
- `ssize_t nand_evaluate(nand_t **g, bool *s, size_t m);` – Computes output signals and the circuit’s critical path length.  
- `ssize_t nand_fan_out(nand_t const *g);` – Returns the number of gates connected to a given gate’s output.  
- `void* nand_input(nand_t const *g, unsigned k);` – Returns the source of an input (`bool*` or `nand_t*`).  
- `nand_t* nand_output(nand_t const *g, ssize_t k);` – Iterates over gates connected to a gate’s output.  

Refer to the provided `nand_example.c` for usage examples.  

## Implementation Requirements  
- The evaluation process should compute each signal and critical path length only once.  
- The library must manage memory properly and handle allocation failures gracefully.  
- The implementation must not impose arbitrary limits on circuit size.  

## Submission Guidelines  
- Submit a compressed archive containing:  
  - `nand.c` and any additional `.c`/`.h` files.  
  - A `Makefile` that builds `libnand.so` and compiles `memory_tests.c`.  
- The `Makefile` must support `make clean` and include `.PHONY` targets.  
- Compilation must use `gcc` with `-Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2`.  
- Linking must use `-shared` with memory-wrapping options.  
- The solution will be tested with `valgrind` to ensure proper memory management. 
