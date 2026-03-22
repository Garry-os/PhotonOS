[bits 64]


global _start
_start:
	;; Open /hello.txt
	mov rax, 2
	mov rdi, filename
	mov rsi, 0
	mov rdx, 0
	int 0x80

	mov [fd_out], rax

	;; Read
	mov rax, 0
	mov rdi, [fd_out]
	mov rsi, buffer
	mov rdx, 100
	int 0x80

	mov rbx, rax

	;; Print out the buffer
	mov rax, 1
	mov rdi, 1 ;; STDOUT
	mov rsi, buffer
	mov rdx, rbx
	int 0x80

	;; Close the file
	mov rax, 3
	mov rdi, [fd_out]
	int 0x80

	jmp $

filename: db "/hello.txt", 0

fd_out: resq 1
buffer: resb 100


