[bits 64]

global _start
_start:
	mov rax, 0 ;; Read
	mov rdi, 72
	int 0x80
	jmp $

