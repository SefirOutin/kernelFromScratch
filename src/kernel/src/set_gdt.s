SECTION .text
global set_gdt

gdtr	DW 0	; limit
		DD 0	; base adress

set_gdt:
	mov ax, [esp - 4]
	mov [gdtr], ax
	mov eax, [esp - 8]
	mov [gdtr + 2], eax
	lgdt [gdtr]