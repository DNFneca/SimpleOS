; boot.asm
; This is a basic 32-bit protected mode bootloader


[BITS 16]

start:
    cli                   ; Disable interrupts
    mov ax, 0x1000        ; Set up stack
    mov ss, ax
    mov sp, 0xFFF0
    sti                   ; Enable interrupts

    ; --- 1. Load the Global Descriptor Table (GDT) ---
    lgdt [gdt_descriptor]

    ; --- 2. Enable Protected Mode ---
    mov eax, cr0
    or eax, 0x1           ; Set the PE (Protection Enable) bit
    mov cr0, eax

    ; --- 3. Jump to the 32-bit code segment ---
    jmp 0x08:protected_mode ; 0x08 is the code segment selector (see GDT)

[BITS 32]
protected_mode:
    ; --- 4. Set up 32-bit Data Segments ---
    mov ax, 0x10          ; 0x10 is the data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; --- 5. Call the C Kernel Entry Point ---
    extern kernel_main      ; Declare C function 'kernel_main'
    call kernel_main

    ; --- 6. Halt the system after kernel finishes (shouldn't happen in a real OS) ---
    cli
    hlt

; --- Global Descriptor Table (GDT) ---
gdt_start:
    ; Null Descriptor (Required)
    dd 0x0
    dd 0x0

gdt_code: ; Code Segment Descriptor (Selector 0x08)
    ; Limit (0xFFFF), Base (0x0)
    dw 0xFFFF
    dw 0x0
    ; Base (0x0), Access (0x9A - Present, Ring 0, Executable, Readable), Flags (0xCF - 4KB Granularity, 32-bit)
    db 0x0, 0x9A, 0xCF, 0x0

gdt_data: ; Data Segment Descriptor (Selector 0x10)
    ; Limit (0xFFFF), Base (0x0)
    dw 0xFFFF
    dw 0x0
    ; Base (0x0), Access (0x92 - Present, Ring 0, Writable), Flags (0xCF - 4KB Granularity, 32-bit)
    db 0x0, 0x92, 0xCF, 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; GDT Size - 1
    dd gdt_start               ; GDT Address

times 510 - ($ - $$) db 0 ; Fill the rest of the boot sector with 0s
dw 0xAA55                 ; Boot Signature