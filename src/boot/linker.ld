ENTRY(stage2_start)

SECTIONS {
    . = 0x8400;

    .text : {
        KEEP(*(.text.stage2_init))
        *(.text.stage2_exit)
        *(.text)
        *(.text.*)
    }

    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : { *(.bss*) *(COMMON) }
}
