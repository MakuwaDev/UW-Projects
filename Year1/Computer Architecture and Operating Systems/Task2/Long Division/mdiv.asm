section .text
    global mdiv

mdiv:
    ; Arguments:
    ;   rdi - pointer to array of 64-bit integers (x)
    ;   rsi - size of the array (n)
    ;   rdx - divisor (y)
    ; Return value:
    ;   rax - remainder

    ; Resetting the registries, which we will later need,
    ; we are here using the 32-bit registries, because all we will require
    ; from them is to store zero or one.
    xor     r9d, r9d
    xor     r10d, r10d

    test    rdx, rdx                        ; Checking whether y is positive.
    jns     .divisor_positive               ; If it is, jump forward.

    neg     rdx                             ; Negating y, if it's negative.
    inc     r9d                             ; Setting a flag to 1, so that we know that y was negative.

.divisor_positive:
    ; Checking whether x is negative
    mov     r8, [rdi + rsi * 8 - 8]         ; Getting the most important 64 bits of x.
    test    r8, r8                          ; Checking whether the most important bit is turned on.
    jns     .all_positive                   ; If x is positive, jump forward.

.negate:
    ; This part of the code is used to 
    ; negate x. It will be possibly 
    ; invoked twice, with the second time
    ; being used to negate the result.
    ; Negating a number in U2 includes
    ; flipping all the bits of the given
    ; number and adding 1 to the result.
    mov     rcx, rsi                        ; Setting our "index" into rcx, to later use the loop instruction.

.flip_loop:
    ; This loop flips the bits of x.
    not     QWORD [rdi + rcx * 8 - 8]       ; Flipping the bits of x on an appropriate invterval.
    loop    .flip_loop                      ; Decrementing rcx and looping back, if it's not zero.

.add_loop:
    ; This loop adds 1 to x. Note, that
    ; at the start of the loop, rcx is set
    ; to zero, following the previous loop.
    ; So we are going from least important 
    ; bits of x, since it is 
    ; written in little-endian.
    add     QWORD [rdi + rcx * 8], 1        ; Adding 1 to the appropriate interval.
    inc     rcx                             ; Incrementing rcx, note that this does not affect the carry flag.
    jc      .add_loop                       ; Looping back, if the carry flag was set, to increment x on the next interval.

    ; This portion of the code checks
    ; whether we just negated the input
    ; or the result back. Note, that the
    ; purpose of registry r10d will 
    ; slightly change after the division.
    ; All we need to know at this point,
    ; is that if r10d is not zero, then we
    ; negated the result back
    ; and can jump to the end.
    cmp     r10d, 0                         ; Checking if r10d is not set to zero. This will occur only if we negated the result back.
    jnz     .done                           ; If we are negated the final result and not the input, jump to the end.
    inc     r10d                            ; Setting a flag to 1, so we know that x was negative.

.all_positive:
    ; From now on we know that both x and y are positive. (We may have to negate the results accordingly).
    ; From now on we treat everything as if unsigned, we will worry about eventual overflow later.
    mov     r8, rdx                         ; Moving our y to r8, because rdx is necessary to perform the div instruction.
    xor     edx, edx                        ; This instruction resets the whole rdx, not only edx. It also reduces the size of mdiv.o.
    mov     rcx, rsi                        ; Setting our "index" into rcx, to later use the loop instruction.

.div_loop:
    ; This loop divides x by y, using
    ; the div instruction. As we know
    ; div <op64> does: 
    ; rax = rdx:rax/<op64>, rdx = remainder.
    ; We will use that to our advantage,
    ; dividing the most significant 64-bits
    ; of x will leave the remainder in rdx.
    ; Then we can perform division of 
    ; rdx:rax / r8 which will leave the 
    ; remainder in rdx. And so on...
    mov     rax,  [rdi + rcx * 8 - 8]       ; Moving the most significant bits that are yet to be divided to rax.
    div     r8                              ; Dividing, we know have the result in rax and remainder in rdx.
    mov     [rdi + rcx * 8 - 8], rax        ; Moving the result back into the array.
    loop    .div_loop                       ; Decrementing rcx, looping back if it's not zero.

    cmp     r10d, 1                         ; Checking if x was negative.
    jnz     .remainder_fixed                ; If not, jumping forward.

    neg     rdx                             ; Negating our remainder. Necessary only if x was negative.

.remainder_fixed:
    ; We now change the purpose of r10d.
    ; Instead of being a flag to remember
    ; if x was negative,it will now be a
    ; flag to know if we should negate
    ; the result. Note, that we will negate
    ; the result if and only if r10d will be
    ; equal to 1.This checks out with what we
    ; do in .negate. If we will be there for
    ; the second time then r10d will not be 
    ; zero, so we will jump to the end.
    xor     r10d, r9d                       ; We should negate the result if exactly one out of x or y was negative at the beginning.
    jnz     .negate                         ; Using the flags set by the xor instruction, if r10d is now not zero, we jump to negate the result.

    ; We now know that we don't have to
    ; negate the result. However, as stated
    ; before, we have treated x as an unsigned.
    ; We now have to convert the result back
    ; to a signed number in U2. As we know that
    ; the result is positive, we don't have to
    ; do anything, unless there is an overflow
    ; and we can't store the result in U2.
    mov     r8, [rdi + rsi * 8 - 8]         ; Getting the most important bits of the result.
    test    r8,  r8                         ; Testing them to determine whether the most important bit is set to 1.
    jns     .done                           ; If r8 is not signed (meaning the most important bit is turned off) we jump to the end.

    ; If r8 had the most significant bit
    ; turned on, we call an overflow.
    div     rcx                             ; rcx is equal to zero following the previous (dividing) loop. Dividing by zero will call an overflow.

.done:
    mov     rax, rdx                        ; Relocating the remainder to where the return value should be.
    ret