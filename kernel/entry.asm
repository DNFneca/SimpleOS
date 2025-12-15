; entry.asm
BITS 64
GLOBAL _start


section .multiboot
align 8

multiboot2_start:
    dd 0xe85250d6                   ; Multiboot2 magic number
    dd 0                            ; Architecture: 0 = i386 protected mode
    dd multiboot2_end - multiboot2_start  ; Header length
    dd -(0xe85250d6 + 0 + (multiboot2_end - multiboot2_start))  ; Checksum

    ; Framebuffer tag (optional - remove if you want text mode)
    align 8
framebuffer_tag_start:
    dw 5                            ; Type: framebuffer
    dw 0                            ; Flags
    dd framebuffer_tag_end - framebuffer_tag_start  ; Size
    dd 1024                         ; Width
    dd 768                          ; Height
    dd 32                           ; Depth (bits per pixel)
framebuffer_tag_end:

    ; Module alignment tag (optional)
    align 8
module_align_tag_start:
    dw 6                            ; Type: module alignment
    dw 0                            ; Flags
    dd module_align_tag_end - module_align_tag_start  ; Size
module_align_tag_end:

    ; End tag (required)
    align 8
    dw 0                            ; Type: end tag
    dw 0                            ; Flags
    dd 8                            ; Size

multiboot2_end:


_start:

    mov rdi, 0xb8000
    mov byte [rdi], 'G'
    mov byte [rdi+1], 0x0F
    mov byte [rdi+2], 'R'
    mov byte [rdi+3], 0x0F
    mov byte [rdi+4], 'U'
    mov byte [rdi+5], 0x0F
    mov byte [rdi+6], 'B'
    mov byte [rdi+7], 0x0F

    ; Optional: set up stack if needed
    mov rsp, 0x90000        ; stack at 576 KB

    ; Call C kernel
    extern kernel_main
    call kernel_main

halt:
    hlt
    jmp halt
