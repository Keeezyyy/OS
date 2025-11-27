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


global read_io_word           
read_io_word:
    push ebp
    mov  ebp, esp

    mov  dx, [ebp+8]   

    xor eax, eax
    in  ax, dx
    leave
    ret 


global write_io_byte
write_io_byte:
    push ebp
    mov  ebp, esp

    mov  dx, [ebp+8]   ; ADRESS
    mov  al, [ebp+12]   ; VAL

    out dx, al
    leave
    ret                