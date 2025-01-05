.arm
.cpu        mpcore

.macro FUNCTION name, section=.text
    .section        \section\().\name, "ax", %progbits
    .align          2
    .global         \name
    .type           \name, %function
    .func           \name
    .cfi_sections   .debug_frame
    .cfi_startproc
    \name:
.endm
.macro END_FUNCTION
    .cfi_endproc
    .endfunc
.endm
