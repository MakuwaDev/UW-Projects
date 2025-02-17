# Sparse Files - CRC Checksum Calculation

## Short task summary/translation:

In Linux, files can have "holes," meaning they consist of continuous data fragments separated by empty spaces. For this task, we assume that a sparse file consists of continuous fragments, where:

- Each fragment starts with a **2-byte length** field, indicating the number of data bytes in the fragment.
- The data bytes follow immediately.
- Each fragment ends with a **4-byte offset**, specifying how many bytes to skip from the end of this fragment to reach the next fragment.

The length field is a **16-bit unsigned integer** (little-endian format). The offset field is a **32-bit signed integer** (two’s complement, little-endian format).  
The first fragment starts at the beginning of the file. The last fragment is identified when its offset points to itself. Fragments in the file may be adjacent or overlapping.

## CRC Checksum Calculation

The checksum is computed using a **cyclic redundancy code (CRC)**, considering only the data bytes from each fragment. Data is processed byte by byte.  
The CRC polynomial is provided as a string of `0`s and `1`s, where the leftmost bit represents the highest-degree term.  
The polynomial **must not be a constant value**.

### Example

The polynomial `11010101` corresponds to:  
x⁸ + x⁷ + x⁶ + x⁴ + x² + 1

## Program Specification

Implement an assembly program **`crc`** that computes the CRC checksum of a sparse file:
./crc file crc_poly  

### Parameters:
- `file`: Name of the input file.
- `crc_poly`: The CRC polynomial as a binary string.

### Output:
- The computed CRC checksum as a binary string, followed by a newline (`\n`).
- The program exits with code `0` on success.
- If an error occurs, the program exits with code `1`.

## System Calls

The program must use Linux **system calls**:
- `sys_open`
- `sys_read`
- `sys_write`
- `sys_lseek`
- `sys_close`
- `sys_exit`

### Error Handling

- The program must validate its input parameters.
- If an error occurs (invalid arguments or system call failure), it must exit with **code 1**.
- The program must **explicitly close the file descriptor** before exiting.

## Buffering for Efficiency

To improve performance, **buffered reading** should be used. The optimal buffer size should be chosen for efficient disk access.  
A comment should indicate the chosen buffer size and the reasoning behind it.

## Submission Instructions

Submit the solution as a file named **`crc.asm`**.

### Compilation

The program will be compiled using:
