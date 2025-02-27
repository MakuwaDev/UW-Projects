# Long Integer Division

## Short task summary/tranlsation:
Implement the following function in assembly, callable from C:

```c
int64_t mdiv(int64_t *x, size_t n, int64_t y);
```

The function performs integer division with remainder.

## Function Behavior
- `x` is a pointer to a non-empty array of `n` 64-bit integers, representing the dividend in **little-endian** order (least significant word first).
- `y` is the divisor.
- The function stores the quotient back into `x` and returns the remainder.
- The function must handle signed numbers using two's complement representation.

## Edge Cases & Error Handling
- If the quotient overflows, the function must trigger an interrupt (SIGFPE), mimicking the behavior of `div` and `idiv` instructions.
- Division by zero must also trigger an interrupt.

## Compilation & Usage
To compile and test the implementation:

```sh
gcc -c -Wall -Wextra -std=c17 -O2 -o mdiv_example.o mdiv_example.c
gcc -z noexecstack -o mdiv_example mdiv_example.o mdiv.o
```

The assembly file should be compiled with:

```sh
nasm -f elf64 -w+all -w+error -o mdiv.o mdiv.asm
```

## Error Handling

    The program validates input correctness in real time.
    Errors are reported to standard error output.
    The program must exit with code 0.
