SECTION .data

gdtr	DW 0	; limit
		DD 0	; base adress

SECTION .text
global load_gdt

load_gdt:
	mov ax, [esp + 4]		; limit
	mov [gdtr], ax			; put limit in gdtr descriptor
	mov eax, [esp + 8]		; base address
	mov [gdtr + 2], eax		; put base address in gdtr descriptor
	lgdt [gdtr]

load_tss:
	mov eax, 0x28			; TSS selector: 0x28 >>> 3 = 5
	ltr ax

reload_code_segment:
	jmp 0x08:reload_data_segments	; code segment selector: 0x08 >> 3 = 1

reload_data_segments:
	mov ax, 0x10			; data segment selector: 0x10 >> 3 = 2
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
