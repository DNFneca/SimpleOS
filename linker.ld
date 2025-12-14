ENTRY(_start)

SECTIONS
{
  . = 1M;

  .multiboot2_header : {
    *(.multiboot2_header)
  }

  .text : {
    *(.text*)
  }
}