section .text
    global _start

    ; Define buffer sizes. 65539=2^16-1+4 allows for reading every fragment 
    ; with 2 sys_reads assuming sys_read would always read the exact amount specified.
    %define BUFFER_SIZE 65539
    %define LENGTH_SIZE 2

    ; Define system call numbers for system functions used.
    %define sys_open  2
    %define sys_read  0
    %define sys_write 1
    %define sys_lseek 8
    %define sys_close 3
    %define sys_exit  60

    %define endl 10

section .bss
    ; Allocate memory for buffer.
    buffer resb BUFFER_SIZE

    ; Allocate memory for lookup table used to calculate CRC.
    lookup_table resq 256

section .text

_start:
    ; Check if exactly 2 parameters passed. 
    ; Address to the program name is considered parameter no. 1
    mov rcx, [rsp]
    cmp rcx, 3
    jne .error

    ; Get address of binary string from parameter.
    ; Prepare r13 and rcx to store crc poly and its length.
    mov rax, [rsp + 24]
    xor r13, r13
    xor ecx, ecx

.convert_loop:
    ; Get current byte.
    ; If null, string ended.
    mov dl, [rax]
    cmp dl, 0
    je .polynomial_calculated

    ; Handle this digit.
    shl r13, 1      ; Make space for this digit.
    cmp dl, '0'     ; If it's less than zero
    jb .error       ; jump to error.
    cmp dl, '1'     ; If it's greater than one
    ja .error       ; also jump to error.
    jne .skip       ; If this digit is zero, continue.
    or r13, 1       ; If it's on, set the according bit.

.skip:
    ; Update appropriate values and loop back.
    inc rax
    inc cl
    jmp .convert_loop

.polynomial_calculated:
    ; Check if polynomial degree is zero, which is considered by us to be invalid.
    cmp rcx, 0
    je .error

    ; Check if polynomial is too long.
    cmp rcx, 64
    jg .error

    ; Shift crc poly left to occupy k most significant bits if k - polynomial degree.
    ; So shift by 64 - k. Then turn rcx to its proper form.
    neg cl
    add cl, 64
    shl r13, cl
    sub cl, 64
    neg cl

    ; Prepare for generating lookup table.
    movzx r9, cl                     ; Store polynomial degree.
    lea rdi, [rel lookup_table]      ; Store the table address.
    xor r10b, r10b                   ; Reset current byte value.
    mov r15, 1                       ; Will be used for shlx.

.next_byte:
    ; Calculate crc remainder for current byte.
    ; It will be stored in the k most significant bits.
    movzx rax, r10b     ; Copy the byte
    shl rax, 56         ; and shift it to most significant bits.
    mov r8b, 8          ; Set current bit (if not shifted and counting from one).

.next_bit:
    ; Process this bit.
    bt rax, 63          ; Check if the highest bit (current) is on.
    shlx rax, rax, r15  ; shlx doesn't allow immiediete values, doesn't affect flags.
    jnc .no_xor         ; If the bit was on, skip the iteration.

    ; Xor with the polynomial.
    ; This method uses only one registry to hold both crc result and remainder.
    ; For each bit we shift it left, and if it was on, then we xor it with crc poly.
    ; After 8 iterations the crc remainder will be in k most significant bits of rax.
    xor rax, r13

.no_xor:
    ; Update current bit and loop back if necessary.
    dec r8b
    jnz .next_bit

    ; Store crc remainder in lookup_table.
    mov [rdi], rax
    add rdi, 8

    ; Update current byte and loop back if necessary.
    inc r10b
    jnz .next_byte

    ; Open file.
    mov eax, sys_open       ; System call number.
    mov rdi, [rsp + 16]     ; Pointer to file.
    xor esi, esi            ; Flags (0 for read-only).
    syscall

    ; Check for error.
    cmp rax, 0
    jl .error

    ; Set appropriate values used while processing the file.
    ; rbx will hold how the next 64-bits should be affected after xoring
    ; latest byte during crc calculations.
    xor ebx, ebx                    ; Reset rbx.
    mov r12, rax                    ; Store file descriptor.
    xor r14, r14                    ; Reset position of current fragment.
    xor r15, r15                    ; Reset current position.
    lea rbp, [rel lookup_table]     ; Get pointer to lookup table.
    lea r13, [rel buffer]           ; Get pointer to buffer.

.next_fragment:
    ; Prepare for reading fragment length in loop.
    mov ecx, LENGTH_SIZE    ; Set bytes to read.
    xor r10b, r10b          ; Set bytes read.

.length_loop:
    push rcx                    ; Store rcx as sys_read affects it.

    ; Read fragment length from file.
    mov eax, sys_read           ; System call number.
    lea rsi, [r13 + r10]        ; Buffer to store data.
    mov edx, ecx                ; Amount of bytes left to read.
    mov rdi, r12                ; File descriptor.
    syscall

    pop rcx

    ; Check for error.
    cmp rax, 0
    jle .error

    ; Update affected values and loop back if necessary.
    add r10, rax        ; Update bytes read.
    sub rcx, rax        ; Update bytes to read.
    jnz .length_loop    ; If not all data read yet, loop back.

    ; Update current position and get fragment length.
    add r15, r10
    movzx ecx, word [r13]

    ; Prepare for reading fragment data.
    ; Add 4 to length in order to (hopefully) read offset with the same syscall.
    add ecx, 4
    xor r10, r10                ; Set bytes read.

.read_loop:  
    push rcx                    ; Store rcx as sys_read affects it.

    ; Read fragment data from file.
    mov eax, sys_read           ; System call number.
    lea rsi, [r13 + r10]        ; Buffer to store read data.
    mov edx, ecx                ; Amount of bytes left to read.
    mov rdi, r12                ; File descriptor.
    syscall

    pop rcx

    ; Check for error.
    cmp rax, 0
    jle .error

    ; Update affected values and loop back if necessary.
    add r10, rax        ; Update bytes read. 
    sub rcx, rax        ; Update bytes to read.
    jnz .read_loop      ; If not all data read yet, loop back.

    ; Prepare for calculating crc on this fragment.
    add r15, r10        ; Update current position.
    mov rcx, r10        ; Store bytes read in rcx.
    sub rcx, 4          ; Subtract 4, not to interfere with offset data.
    xor r8, r8          ; Reset bytes processed.

    ; Check if fragment has length zero. If it does, skip calculations.
    cmp rcx, 0
    je .fragment_calculated

.crc_loop:
    ; Process current byte, after each byte k most significant bits of rbx
    ; contain crc remainder if this was the last byte, where k is the polynomial degree.
    mov al, [r13 + r8]          ; Get current byte.
    mov r10, rbx                ; Copy how the next 64 bits were affected.
    shr r10, 56                 ; Extract the most significant byte.
    xor r10b, al                ; xor current byte with how it was affected.
    shl rbx, 8                  ; Shift to sustain the correct value of rbx.
    xor rbx, [rbp + 8 * r10]    ; Affect next 64 bits of data accordingly to current byte.
    inc r8                      ; Update byte index.
    loop .crc_loop              ; Loop back.

.fragment_calculated:
    ; Update current position by offset.
    movsx rax, DWORD [r13 + r8]     ; Move offset to rax.
    add r15, rax                    ; Update current position.
    js  .error                      ; Current position is negative.

    ; Check if offset points to current fragment. If so, we are done.
    cmp r14, r15
    je .done

    ; Update current position in file by lseek.
    mov eax, sys_lseek      ; System call number.
    mov rdi, r12            ; File descriptor.
    xor edx, edx            ; Seek from file beginning.
    mov rsi, r15            ; Current position.
    syscall

    ; Check for error.
    cmp rax, r15
    jne .error

    ; Set current fragment position and repeat.
    mov r14, rax
    jmp .next_fragment

.done:
    ; Prepare for converting remainder to string by shifting rbx appropriately.
    mov cl, r9b     ; Get remainder length,
    neg cl          ; negate it
    add cl, 64      ; and add 64
    shr rbx, cl     ; to shift by 64 - length.
    mov cl, r9b  ; Get remainder length in rcx.

.next_digit:
    ; Get next digit.
    mov al, bl                      ; Get 8 least significant bits
    and al, 1                       ; and extract the least significant one.
    add al, '0'                     ; Convert it to ascii character.
    mov [r13 + rcx - 1], al         ; Store it in adequate place in buffer.
    shr rbx, 1                      ; Shift by 1, to get next digit
    loop .next_digit                ; and loop back.

    ; Set EOL character at the end of string.
    mov [r13 + r9], byte endl
    inc r9

.write_loop:
    ; Write remaining bytes to stdout.
    mov eax, sys_write          ; System call number.
    mov edi, 1                  ; STDOUT file descriptor.
    lea rsi, [r13 + rcx]        ; Buffer with string to write.
    mov edx, r9d                ; Amount of bytes left to read.
    syscall

    ; Check for error.
    cmp rax, 0
    jle .error

    ; Update affected values and loop back.
    add rcx, rax        ; Update bytes written.
    sub r9, rax         ; Update bytes to write.
    jnz .write_loop     ; If not all bytes written, loop back.

    ; Close the file.
    mov eax, sys_close  ; System call number.
    mov rdi, r12        ; File descriptor.
    syscall

    ; Check for error.
    cmp rax, 0
    jl .error

    ; Exit program with code 0.
    mov eax, sys_exit       ; System call number.
    xor edi, edi            ; Exit code 0.
    syscall

.error:
    ; Close the file.
    mov eax, sys_close      ; System call number.
    mov rdi, r12            ; File descriptor.
    syscall

    ; Exit program with code 1.
    mov eax, sys_exit       ; System call number.
    mov edi, 1              ; Exit code 1.
    syscall