SECTION .text

; void enable_paging(uint32 page_directory)
global enable_paging

enable_paging:
	mov eax, [esp + 4]		; load page directory addr
	mov cr3, eax			; and put it to cr3

	mov eax, cr0			; load cr0
	or eax, 0x80000001		; set protected mode, paging bits
	mov cr0, eax			; put it back