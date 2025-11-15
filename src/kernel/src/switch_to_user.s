SECTION .text

global switch_to_user_mode

switch_to_user_mode:
	cli					; disables int

	mov ax, 0x23		; loads user data segments: 0x23 >> 3 = 4 / 0x03 for user privilege
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; arguments for iret
	mov eax, [ebp + 8]
	mov esi, [ebp + 4]
	push 0x23			; SS (user data/stack segment)
	push eax			; ESP (user stack)
	pushf				; EFLAGS
	push 0x1B			; CS (user code segment) 0x18 | 0x3 = 0x1b
	push esi			; EIP (next instruction)
	iret
