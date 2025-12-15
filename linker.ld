ENTRY(_start)

SECTIONS
{
    . = 1M;  /* Load kernel at 1 MB */

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss COMMON) }
}
