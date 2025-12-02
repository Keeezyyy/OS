ENTRY(stage2_start)

SECTIONS {
    . = 0x8200;

    .text : {
        KEEP(*(.text.stage2_init))
        *(.text.stage2_exit)
        *(.text.stage2_mmap)
        *(.text)
        *(.text.*)
    }

    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : { *(.bss*) *(COMMON) }
}
