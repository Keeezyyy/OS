[BITS 32]


section .text.stage2_exit
global exit
exit:

    mov [entry_address], eax


    push eax
;paging for long mode init

;PML4 -> PDPT link
    mov eax, PDPT
    or eax, 0b11
    mov [PML4 + 0*8], eax
    mov dword [PML4 + 0*8 + 4], 0

;PDPT -> PD link
    mov eax, PD
    or eax, 0b11           
    mov [PDPT + 0*8], eax
    mov dword [PDPT + 0*8 + 4], 0

; PD[0] = 2MiB-Page für 0x00000000–0x001FFFFF
    mov eax, 0x00000083          
    mov [PD + 0*8], eax
    mov dword [PD + 0*8 + 4], 0

    mov eax, PML4            
    mov cr3, eax

;gdt set
    lgdt [gdt64_descriptor]


; inti long mode  (Paging on)
    mov eax, cr4
    or eax, 1 << 5           
    mov cr4, eax

enable_long_mode:
    mov ecx, 0xC0000080      
    rdmsr
    or eax, 1 << 8           
    wrmsr

;activae paging 
    mov eax, cr0
    or eax, 1 << 31          
    mov cr0, eax

    

    jmp 0x08:long_mode_entry
          
[BITS 64]
long_mode_entry:
    mov ax, 0x10      
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax


;TODO implement better dynamic stack detection
;WIP read stack from kenel elf
    mov rsp, 0x200000   

    ; Annahme: stage2_main hat Kernel-Entry-Adresse in RAX (oder sonst wo) gespeichert
    ; Beispiel: RAX = Kernel-Entry
    mov rax, [entry_address]
final_exit:
    jmp rax



; 64 BIT gdt 
gdt64_start:
    dq 0                          

    dq 0x00209A0000000000

    dq 0x0000920000000000
gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64_start - 1
    dd gdt64_start

entry_address: dq 0



;find better option either better stage 1 bootloader to load larger stage 2 or other option
PML4 equ 0x200000
PDPT equ 0x210000
PD   equ 0x220000