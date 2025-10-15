SECTION .multiboot_header
    align 8
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

SECTION .text
global _start
_start:
    mov esp,  stack_top
    push ebx
    call kmain
.hang:
    hlt
    jmp .hang

SECTION .bss
    align 16
    stack_bottom:
        resb 16384      ; 16 KiB
    stack_top: