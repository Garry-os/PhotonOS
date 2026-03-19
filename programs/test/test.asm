[bits 64]

global _start
_start:
	mov rax, 1 ;; Write
	mov rdi, 1 ;; stdout
	mov rsi, msg
	mov rdx, msg_len 
	int 0x80

	jmp $

msg: db "Hello World!", 0xa
msg_len equ $ - msg

