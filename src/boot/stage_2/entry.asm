[BITS 16]

extern stage2_main
extern printf
section .text.stage2_init
global stage2_start



stage2_start:
    cli               

    in   al, 0x92
    or   al, 00000010b
    out  0x92, al

    lgdt [gdt_descriptor]

    mov eax, cr0
    or  eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode_entry

[BITS 32]
protected_mode_entry:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov esp, stack_top    

    call stage2_main


global halt
halt:
    push halt_msg
    call printf
    cli
    hlt
    jmp halt

gdt_start:
    dq 0x0000000000000000   

    dq 0x00CF9A000000FFFF

    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

halt_msg:
    db "CPU Halted", 0


section .bss
align 16
stack_bottom:
    resb 4096         
stack_top:
