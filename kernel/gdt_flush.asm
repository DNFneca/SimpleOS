; FILE: gdt_flush.asm

[bits] 64

global gdt_flush


gdt_flush:
    ; Argument (GDT Pointer) is in RDI (System V AMD64 ABI)
    lgdt [rdi]        ; Load the 64-bit GDT pointer

    ; Reload Data Segments (Index 2 = 0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Reload Code Segment (Index 1 = 0x08)
    ; We push the Code Segment Selector and the Return Address to the stack
    ; and use RETFQ (Far Return) to pop them into CS and RIP.

    pop rdi           ; Save return address (from function call)
                      ; NOTE: This assumes standard call stack.
                      ; If called from C, stack has [Return RIP].

    mov rax, 0x08     ; Kernel Code Selector
    push rax          ; Push CS
    push rdi          ; Push RIP (Return Address)

    retfq             ; Pops RIP and CS, effectively doing a far jump