[BITS 32]


global read_io_byte
read_io_byte:
    push ebp
    mov  ebp, esp

    mov  dx, [ebp+8]   

    xor eax, eax
    in  al, dx

    leave
    ret                


global write_io_byte
write_io_byte:
    push ebp
    mov  ebp, esp

    mov  dx, [ebp+8]   ; ADRESS
    mov  al, [ebp+8]   ; VAL

    out dx, al
    leave
    ret                