[bits 32]

section .text
	global _start

_start:
	xor eax, eax
	xor ebx, ebx
	jmp multiboot_entry

magic_fields:
	align 8
	dd 0xE85250D6							; magic number
	dd 0									; architecture
	dd magic_fields_end - magic_fields		; header length
	dd 0x100000000 - (0xE85250D6 + (magic_fields_end - magic_fields))	; checksum
magic_fields_end:

address_tag:
	align 8
	dw 2									; type: address tag
	dw 0									; flags
	dd address_tag_end - address_tag		; size
	dd magic_fields							; header address
	dd _start								; load addr of code entry
	dd 0									; end addr of code (here assumes its the whole iso)
	dd bss_end								; end addr of bss section
address_tag_end:


entry_addr_tag:
	align 8
	dw 3									; type: entry address tag
	dw 0									; flags
	dd entry_addr_tag_end - entry_addr_tag	; size
	dd multiboot_entry						; entry address into kernel
entry_addr_tag_end:	

; flags_tag:
; 	align 8
; 	dw 4									; type: console type
; 	dw 0									; flags
; 	dd 12									; size
; 	dd 3									; EGA text support
; flags_tag_end:

; framebuffer_tag:
; 	align 8
; 	dw 5									; type: framebuffer
; 	dw 0
; 	dd 20									; size
; 	dd 
; 	dd 200
; 	dd 0
; framebuffer_tag_end:

	align 8
	dw 0									; end of tags struct
	dw 0
	dd 8

section .bss
	align 16
	stack_bottom:						; end of stack
	resb 16384							; 16kB of stack
	stack_top:							; start of stack (grows downwards)
bss_end:

multiboot_entry:
	mov esp, stack_top						; set stack for the kernel 

	cmp eax, 0x36d76289						; eax must have this magic number given by grub
	jne	endk

	mov byte [0xb0000], 69

	loop:
		jmp loop

endk:
	ret
