[BITS 32]

; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained

global read_disk_asm
read_disk_asm:
    push ebp
    mov  ebp, esp

    pusha
    mov eax, [ebp+8]    
    mov cl, [ebp+12]    
    mov edi, [ebp+16]   
    call ata_lba_read
    popa

    leave
    ret  


ata_lba_read:
    pushf
    push eax
    push ebx
    push ecx
    push edx
    push edi

    mov ebx, eax        

    mov dx, 0x01F6
    mov eax, ebx
    shr eax, 24
    or  al, 0xE0        
    out dx, al

    mov dx, 0x01F2
    mov al, cl
    out dx, al

    mov dx, 0x01F3
    mov eax, ebx
    out dx, al

    mov dx, 0x01F4
    mov eax, ebx
    shr eax, 8
    out dx, al

    mov dx, 0x01F5
    mov eax, ebx
    shr eax, 16
    out dx, al

    mov dx, 0x01F7
    mov al, 0x20
    out dx, al

.wait:
    in al, dx
    test al, 8
    jz .wait

    movzx ecx, cl
    mov eax, 256
    mul ecx            
    mov ecx, eax

    mov dx, 0x1F0
    rep insw           

    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    popf
    ret