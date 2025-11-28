[org 0x7C3E]

start:
init:
    mov [bootDrive], dl
    cli
    mov ax, 0x9000
    mov ss, ax
    mov sp, 0xFFFF
    sti
    cld
_load_FAT_table:
    ; copieng 14 secors 
    ; starting at sector 19
    ; header FAT data 
    mov cx, 19
    mov [amountOfSectors], 14
    call _read_from_disk_into_buffer

    ; error its 224 enries 
    ;printing the data region as a string
    mov bx, 14        ; Loop counter 
    xor cx, cx        ; Start offset
    mov si, [copyTo]  ; base address


_loop:
    mov dx, si ; dx = ram starting address
    add dx, cx ; adding dx += cx

    mov ax, dx  ; ax = dx (for prinitng)
    ;call print

    ;calling cmp string function
    ;if ax == 1 -> correct
    ;if ax != 1 -> incorrect

    call _cmp_str_to_filename

    xor ax, ax

    mov al, [stringCMP]


    cmp ax, 1
    je _fileFound

    mov ax, dx


    add cx, 32     ; CX += 32

    dec bx
    jnz _loop

    jmp $

_fileFound:
    mov si, 0x1A
    mov bx, dx ; pointer to entry

    mov [entryPTR], bx
    mov ax, [bx + si]





    mov [FATEntryNum], ax
    ; [FATEntryNum] = starting sector 
    mov bx, 0x7C00
    ; bx = PBM starting pointer

;------------------------------------------------
;                         //0x0e                  //0x0b
;return ((uint32_t)b->ReservedSectors * b->BytesPerSector);
_calc_FAT_POS:
    mov si, 0x0E
    mov ax, [bx + si]

    mov si, 0x0B
    mov cx, [bx + si]

    mul ax, cx
    mov [dataStart], ax

    push cx
            ; copieng 7 secors 
            ; starting at sector 1
             ; header FAT data 
        mov cx, 0x9400

        mov [copyTo], cx
        mov cx, 0
        mov [amountOfSectors], 3
        call _read_from_disk_into_buffer
    pop cx
;---------------------------------------------------

;------------------------------------------------
;                                                //0x10                              //0x16                      //0x0b
;uint32_t sizeOfAllFATsInBytes = (uint32_t)b->FileAllocationTableCount * b->SectorsPerFileAllocationTable * b->BytesPerSector;
;return (FAT_Start + sizeOfAllFATsInBytes);
_calcRoot_Start:
    mov ax, [bx+10h]   
    mov cx, [bx+16h]   
    mul cx             
    mov cx, [bx+0Bh]   
    mul cx             
    add ax, [dataStart]
    mov [dataStart], ax
;------------------------------------------------

;------------------------------------------------
;return (rootStart + (uint32_t)(b->MaxRootDirectoryEntries * 32));
_calc_data_start:
    mov ax, [bx + 0x11]

    mov cx, 32

    mul ax, cx


    mov cx, [dataStart]
    add ax, cx
    mov [dataStart], ax
;------------------------------------------------


 _read_entry:
    mov bx, [copyTo]
    add bx, 512

    mov cx, [FATEntryNum]
    mov ax, cx
   
    mov ax, 0x8200
    mov [copyTo], ax

    mov ax, cx

.readLoop:
    call getClusterEntry
    ; -> returns index in cx
    

    push cx
        mov cx, ax  ; -> do while 
                    ; ax holds entrnum of last iteration
                    ;
        call read_copy_sector
        ; -> reads 
    pop cx

    mov ax, cx 

    cmp ax, 0xFEF

    jbe .readLoop


    jmp 0x0000:0x8400

;bx entry pointer
;cx offset
 read_copy_sector:  
     pusha   
         sub cx, 2
       
         xor dx, dx
         mov ax, [dataStart]
         mov bx, 512
         div bx
         add ax, cx
         mov bx, ax      ; function parameter
                        ; bx -> 
        ; calc adr on disk and converting in sector offset e.g. 
        ; 0x0200 -> 1
        ; 0x0400 -> 2
         mov ax, [copyTo]
         add ax, 512
         mov [copyTo], ax

        ; ax -> ram adresse of buffer 
        ; first start in copyto + (dx * 512)
        ; save back to copyTo for next buffer write


         mov cx, bx          ; cx -> sector offst from 0x0 in sectors(512B)
                            ;[copyTO] -> pointer to buffer for copieng
         mov [amountOfSectors], 1

         call _read_from_disk_into_buffer



     popa
     ret


;uint16_t getClusterEntry(FAT12 *f, uint16_t index)
;bx FAT table pointer
;ax index
;cx return
;------------------------------------------------

getClusterEntry:
    push ax            ; index sichern
    push bx
    push dx
    push si

    ;------------------------------------------------------------
    ; byte_index = index + index/2   (entspricht (index*12)/8)
    ;------------------------------------------------------------
    mov dx, ax         ; DX = index
    shr dx, 1          ; DX = index / 2
    add dx, ax         ; DX = index + index/2 = byte_index

    ; SI = BX + byte_index = Adresse des Eintrags in der FAT
    mov si, bx
    add si, dx

    ; 16 Bit aus FAT laden: w = *(uint16_t*)(FAT + byte_index)
    mov dx, [si]

    ;------------------------------------------------------------
    ; Wenn index gerade:
    ;   entry = w & 0x0FFF
    ; Wenn index ungerade:
    ;   entry = w >> 4
    ;
    ; Das ist algebraisch äquivalent zu deiner C-Version mit
    ; bit_start, byte_index, bit_offset und 12-Bit-Shift.
    ;------------------------------------------------------------
    test ax, 1         ; ursprünglichen index (in AX) testen
    jnz .oddEntry

.evenEntry:
    and dx, 0x0FFF     ; nur die unteren 12 Bits
    jmp .done

.oddEntry:
    shr dx, 4          ; obere 12 Bits nehmen

.done:
    mov cx, dx         ; Rückgabewert in CX

    pop si
    pop dx
    pop bx
    pop ax
    ret



;------------------------------------------------
;void isSame(char *a, char *b, uint8_t *out)
;------------------------------------------------------------------------------------------------------------------------
_cmp_str_to_filename:
    pusha

    mov dx, ax              ; dx = Pointer auf FAT-Dateiname
    mov bx, fileToSearch            ; bx = Pointer auf Vergleichs-String

    mov si, 0               ; Index = 0
    mov cx, 11              ; 11 Zeichen vergleichen
    mov byte [stringCMP], 1 ; erstmal TRUE annehmen

_cmp_loop:
    push bx
    mov bx, dx
    mov al, [bx + si]       ; FAT-Name[si]

    pop bx

    cmp al, [bx + si]
    jne _cmp_incorrect

    inc si                  ; nächstes Zeichen
    loop _cmp_loop          ; cx--, solange cx != 0 -> weiter

    jmp _cmp_end            ; alles gleich

_cmp_incorrect:
    mov byte [stringCMP], 0xFE

    ; optional: ein 'X' ausgeben, um Fehler zu markieren

_cmp_end:
    popa
    ret
;------------------------------------------------------------------------------------------------------------------------




;---------------------------------------------
; print_hex8_fast: AL = byte to print
; destroys: AL, AH
;---------------------------------------------








;ah -> int number(0x02 -> read sector)
;dl -> disk number

;al -> sector amount
;ch -> cylinder
;CL = Bits 0–5: Sektor, Bits 6–7: Zylinder high
;dh head

;ES:BX = Buffer

;cf -> return value !bool

;input cx -> sector 
_read_from_disk_into_buffer:
    mov bx, cx 
    call _convert_LBA_to_CHS

    push cx
    push dx
    
    mov ax, [cylinder]      ; AX = cylinder (0..1023)
    mov ch, al              ; CH = low 8 bits des Zylinders

    mov al, ah              ; AL = high bits des Zylinders (Bits 8-15)
    shl al, 6               ; Bits 0-1 -> Bits 6-7 (für CL)
    and al, 0xC0            ; nur Bits 6-7 behalten

    mov cl, [sector]        ; Sektor (1..63)
    and cl, 0x3F            ; nur 6 Bits (Bits 0-5)
    or  cl, al              ; Zylinder-high-Bits (Bits 6-7) dazupacken

    ; Kopf (Head) in DH
    mov dh, [head]

    ; Laufwerk in DL
    mov dl, [bootDrive]

    ; BIOS-Funktion: AH = 02h (lesen), AL = Anzahl Sektoren
    mov ah, 0x02
    mov al, 1               ; genau 1 Sektor lesen

    ; ES:BX muss hier bereits auf den Buffer zeigen!

    xor ax, ax
    mov es, ax
    mov bx, [copyTo]


    mov ah, 0x02
    mov al, [amountOfSectors]
    int 0x13
    
    xor ax, ax
    setc al     ; AL = 1 wenn CF=1 sonst 0

    pop dx
    pop cx

    ret




; void lba_to_chs(uint32_t lba, uint16_t *c, uint16_t *h, uint16_t *s)
; {
;     const uint8_t heads   = 2;
;     const uint8_t sectors = 18;

;     *c = lba / (heads * sectors);
;     uint32_t temp = lba % (heads * sectors);

;     *h = temp / sectors;
;     *s = (temp % sectors) + 1;   // sectors are 1-based
; }

;bx secor number
_convert_LBA_to_CHS:
    pusha

    mov cx, 36          ; heads * sectors = 2 * 18

    mov ax, bx
    xor dx, dx
    div cx              ; AX = LBA / 36  => cylinder
                        ; DX = remainder => temp
    mov [cylinder], ax

   ; temp / sectors => head
    mov ax, dx
    mov cx, 18
    xor dx, dx
    div cx              ; AX = head, DX = remainder
    mov [head], ax

    ; (temp % sectors) + 1 => sector
    mov ax, dx
    inc ax
    mov [sector], ax

    popa
    ret
    jmp $





bootDrive db 0xFF
stringCMP db 0
amountOfSectors db 0
head dw 0
cylinder dw 0
sector dw 0
copyTo dw 0x8000


dataStart dw 0
FATEntryNum db 0
entryPTR dw 0


alocStart dw 0

fileToSearch db 'ENTRY   BIN'
times 448-($-$$) db 0
dw 0xAA55