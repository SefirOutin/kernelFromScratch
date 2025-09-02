[bits 32]

section .multiboot_header
magic_fields:
	align 8
	dd 0xE85250D6							; magic number
	dd 0									; architecture
	dd last_tag_end - magic_fields		; header length
	dd 0x100000000 - (0xE85250D6 + (last_tag_end - magic_fields))	; checksum
magic_fields_end:

mbi_request_tag:
	align 8
	dw 1									; type multiboot2 information request
	dw 0									; flags
	dd mbi_request_tag_end - mbi_request_tag; size
	dd 8									; mdi_tag_types array
mbi_request_tag_end:

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

flags_tag:
	align 8
	dw 4									; type: console type
	dw 0									; flags
	dd 12									; size
	dd 0b11									; EGA text support (console_flags)

framebuffer_tag:
	align 8
	dw 5									; type: framebuffer
	dw 0
	dd 20									; size
	dd 800
	dd 600
	dd 32

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
	xor eax, eax
	xor ebx, ebx
	jmp multiboot_entry


multiboot_entry:
	mov esp, stack_top						; set stack for the kernel
	and esp, 0xFFFFFFF0						; align 16bytes - mask to round down

	cmp eax, 0x36d76289						; eax must have this magic number given by grub
	jne	endk

	; mov byte [0xb8000], 69
	push ebx								; push grub struct (1st arg x86 conv)
	extern kernel_main
	call kernel_main
	add esp, 4								; clean stack

endk:
	ret
