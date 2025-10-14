SECTION .multiboot_header
    .align 8
    header_start:
        dd 0xe85250d6                   ;magic number
        dd 0                            ;protected mode code
        dd header_end - header_start    ;header length
    
        ;checksum
        dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) 
    
        ; required end tag
        dw 0    ;type
        dw 0    ;flag
        dd 8    ;size
    header_end:


global _start:
    mov esp,  stack_top
    .align 16
    push ebx
    call kmain
    hlt


SECTION .bss
    .align 16
    stack_bottom:
        .skip 16384      ; 16 KiB
    stack_top: