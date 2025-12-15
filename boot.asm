

[BITS 16]
[ORG 0x7C00]
start:

    ; --- REAL MODE TEST ---
    mov ah, 0x0E
    mov al, 'R'
    int 0x10

    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00

    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax


    jmp 0x08:pmode

; --------------------
; 32-bit protected mode
; --------------------
[BITS 32]
pmode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; PROTECTED MODE (32-bit)
    mov byte [0xB8000], 'P'     ; character
    mov byte [0xB8001], 0x07    ; attribute (light grey on black)

    mov esp, 0x90000

    call setup_paging

    mov eax, cr4
    or eax, 1 << 5       ; PAE
    mov cr4, eax

    mov ecx, 0xC0000080  ; EFER
    rdmsr
    or eax, 1 << 8       ; LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31      ; PG
    mov cr0, eax

    jmp 0x18:long_mode

; --------------------
; Paging (2MB identity)
; --------------------
setup_paging:
    mov edi, 0x8000
    xor eax, eax
    mov ecx, 4096
    rep stosb

    ; PML4[0] -> PDPT
    mov dword [0x8000], 0x9003

    ; PDPT[0] -> PD
    mov dword [0x9000], 0xA003

    ; PD entries (2MB pages)
    mov edi, 0xA000
    mov eax, 0x83        ; Present | Write | PS
    mov ecx, 512

.map:
    mov [edi], eax
    add eax, 0x200000
    add edi, 8
    loop .map

    mov eax, 0x8000
    mov cr3, eax
    ret

; --------------------
; 64-bit long mode
; --------------------
[BITS 64]
long_mode:

    ; LONG MODE (64-bit)
    mov byte [0xB8000], 'L'
    mov byte [0xB8001], 0x0A    ; light green


    mov ax, 0x20
    mov ds, ax
    mov ss, ax

    mov rsp, 0x900000

.hang:
    hlt
    jmp .hang

; --------------------
; GDT
; --------------------
gdt:
dq 0

; 32-bit code
dq 0x00CF9A000000FFFF
; 32-bit data
dq 0x00CF92000000FFFF
; 64-bit code
dq 0x00209A0000000000
; 64-bit data
dq 0x0000920000000000


gdt_end:

gdt_desc:
dw gdt_end - gdt - 1
dq gdt

times 510-($-$$) db 0
dw 0xAA55
