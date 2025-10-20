[bits 32]

MAGIC_NUMBER equ 0xE85250D6
HEADER_LENGHT equ last_tag_end - magic_fields

section .multiboot_header
magic_fields:
	align 8
	dd 	MAGIC_NUMBER						; magic number
	dd 0									; architecture
	dd HEADER_LENGHT						; header length
	dd 0x100000000 - (MAGIC_NUMBER + HEADER_LENGHT)		; checksum
magic_fields_end:

; flags_tag:
; 	align 8
; 	dw 4									; type: console type
; 	dw 0									; flags
; 	dd 12									; size
; 	dd 0b10									; EGA text support (console_flags)

; framebuffer_tag:
; 	align 8
; 	dw 5									; type: framebuffer
; 	dw 0
; 	dd 20									; size
; 	dd 160
; 	dd 50
; 	dd 32


last_tag:
	align 8
	dw 0									; end of tags struct
	dw 0
	dd 8
last_tag_end:

section .bss
	align 16
	stack_bottom:						; end of stack
	resb 16384							; 16kB of stack
	stack_top:							; start of stack (grows downwards)
bss_end:

section .text
	global _start

_start:
	mov esp, stack_top						; set up stack
	and esp, 0xFFFFFFF0						; align 16bytes - mask to round down

	cmp eax, 0x36d76289						; eax must have this magic number given by grub
	jne	endk

	push ebx								; push grub struct (1st arg x86 conv)
	extern kernel_main
	call kernel_main
	add esp, 4								; clean stack

endk:
	ret
