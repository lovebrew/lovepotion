.arm

#include "asm_macros.s.h"

#define HAS_PLD

/*
 * memcpy
 *     r0: dst
 *     r1: src
 *     r2: size
 * Requires:
 *      - Unaligned memory access support
 */
FUNCTION xmemcpy
    // Check if there are only 3 bytes to copy or less
    cmp     r2, #3
    bls     4f

    // Check misalignment for "dst"
    ands    r12, r0, #3
    beq     1f

    // Copy "dst" misaligned bytes
    ldrb    r3, [r1], #1
    cmp     r12, #2
    add     r2, r2, r12
    ldrlsb  r12, [r1], #1
    strb    r3, [r0], #1
    ldrccb  r3, [r1], #1
    strlsb  r12, [r0], #1
    sub     r2, r2, #4
    strccb  r3, [r0], #1

1:
    // If dst is 4-byte-aligned, use most optimized routine
    // and return, otherwise copy 8 bytes per 8
    ands    r3, r1, #3
    beq     xaeabi_memcpy
    subs    r2, r2, #8

2:
    // Copy 8 bytes per 8 (possibly unaligned)
    // Different loop on armv5
    bcc     3f
    ldr     r3, [r1], #4
    subs    r2, r2, #8
    ldr     r12, [r1], #4
    stm     r0!, {r3, r12}
    b       2b

3:
    // Between 4 and 7 bytes remaining
    adds    r2, r2, #4
    ldrpl   r3, [r1], #4
    strpl   r3, [r0], #4
    nop

4:
    // Less than 4 bytes remaining
    // <C,N> = r2 % 4
    lsls    r2, r2, #31
    ldrcsb  r3, [r1], #1
    ldrcsb  r12, [r1], #1
    ldrmib  r2, [r1], #1
    strcsb  r3, [r0], #1
    strcsb  r12, [r0], #1
    strmib  r2, [r0], #1
    bx      lr
END_FUNCTION

FUNCTION xaeabi_memcpy
    push    {r4-r10, lr}

    // Check if there more than one cache line to copy
    subs    r2, r2, #32
    bcc     2f

    // Copy a cache line worth of bytes and preload the one 2 or 3 slots further.
    // Split into 2 load/stores
    ldm     r1!, {r3-r6}
#ifdef HAS_PLD
    pld     [r1, #64]
#endif
    ldm     r1!, {r7-r10}
1:
    stm     r0!, {r3-r6}
    subs    r2, r2, #32
    stm     r0!, {r7-r10}
    ldmcs   r1!, {r3-r6}
#ifdef HAS_PLD
    pld     [r1, #64]
#endif
    ldmcs   r1!, {r7-r10}
    bcs     1b

2:
    // Less than 32 bytes to copy
    // <C,N> = (r2 % 32) >> 3
    lsls    r12, r2, #28

    // 16 bytes or more
    ldmcs   r1!, {r3, r4, r12, lr}
    stmcs   r0!, {r3, r4, r12, lr}

    // 8 bytes or more
    ldmmi   r1!, {r3, r4}
    stmmi   r0!, {r3, r4}

    // Less than 8 bytes to copy
    // <C,N> = (r2 % 8) >> 1, Z = r2 % 4
    pop     {r4-r10, lr}
    lsls    r12, r2, #30

    // 4 bytes or more
    ldrcs   r3, [r1], #4
    strcs   r3, [r0], #4
    bxeq    lr

    // Less than 4 bytes remaining
    // <C,N> = r2 % 4
    lsls    r2, r2, #31
    ldrcsh  r3, [r1], #2
    ldrmib  r2, [r1], #1
    strcsh  r3, [r0], #2
    strmib  r2, [r0], #1

    bx      lr
END_FUNCTION
