ENTRY(_start)
. = 1M;
.text :
{
    *(.multiboot_header)
    *(.text*)
}
.rodata :
{
  *(.rodata*)
}
.data :
{
  *(.data*)
}
.bss :
{
  *(COMMON)
  *(.bss*)
}
end = .;
