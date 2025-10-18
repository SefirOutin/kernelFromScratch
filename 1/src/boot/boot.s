SECTION .multiboot_header
    align 8
    header_start:
        dd 0xe85250d6                   ;magic number
        dd 0                            ;protected mode code
        dd header_end - header_start    ;header length
        dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ;checksum 

        
        ;framebuffer_tag_start:
        ;    dw  0x05    ;Type: framebuffer
        ;    dw  0x01    ;Optional tag
        ;    dd  framebuffer_tag_end - framebuffer_tag_start ;size
        ;    dd  1024   ;Width - if 0 we let the bootloader decide
        ;    dd  760  ;Height - same as above
        ;    dd  32 ;Depth  - same as above
        ;framebuffer_tag_end:        
    
        ; required end tag
        dw 0    ;type
        dw 0    ;flag
        dd 8    ;size   
    header_end:
    
 

SECTION .text
global _start
extern kkernel
_start:
    mov esp,  stack_top
    push ebx
    call kkernel
.loop:
    hlt
    jmp .loop

SECTION .bss
    align 16
    stack_bottom:
        resb 16384      ; 16 KiB
    stack_top: