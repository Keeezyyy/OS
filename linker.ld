ENTRY(stage2_start)

SECTIONS {
    . = 0x8400;

    .text : {
        *(.text.stage2_init)   /* <-- dein entry.asm */
        *(.text*)              /* alle anderen danach */
    }

    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : { *(.bss*) *(COMMON) }
}
