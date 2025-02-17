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
