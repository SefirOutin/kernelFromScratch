SECTION .text

global switch_to_user_mode

;	void switch_to_user_mode(void *user_stack_ptr, void *user_entry);
switch_to_user_mode:
	cli					; disables int

	mov ax, 0x23		; loads user data segments: 0x23 >> 3 = 4 / 0x03 for user privilege
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	; mov ss, ax

	; arguments for iret
	mov eax, [esp + 4]	; *user_stack_ptr
	mov esi, [esp + 8]	; *user_entry
	push 0x23			; SS (user data/stack segment)
	push eax			; ESP (user stack)
	pushf				; EFLAGS
	; pop eax
	; or eax, 0x200		; Set IF flag to enable interrupts in user mode
	; push eax
	push 0x1B			; CS (user code segment) 0x18 | 0x3 = 0x1b
	push esi			; EIP (next instruction)
	iret
