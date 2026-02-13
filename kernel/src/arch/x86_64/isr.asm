[bits 64]

;; From isr.c
extern interrupt_handler

section .text

;;
;; Helper macros
;;
%macro ISR_ERROR_CODE 1
isr%1:
	;; Error code is already pushed
	push %1 ; Interrupt number
	jmp isr_common
%endmacro

%macro ISR_NO_ERROR_CODE 1
isr%1:
	push 0 ; A dummy error code
	push %1 ; Interrupt number
	jmp isr_common
%endmacro

%macro save_context 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro restore_context 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

;;
;; Interrupts
;;
; Exceptions reserved by the CPU
ISR_NO_ERROR_CODE 0
ISR_NO_ERROR_CODE 1
ISR_NO_ERROR_CODE 2
ISR_NO_ERROR_CODE 3
ISR_NO_ERROR_CODE 4
ISR_NO_ERROR_CODE 5
ISR_NO_ERROR_CODE 6
ISR_NO_ERROR_CODE 7
ISR_ERROR_CODE    8
ISR_NO_ERROR_CODE 9
ISR_ERROR_CODE    10
ISR_ERROR_CODE    11
ISR_ERROR_CODE    12
ISR_ERROR_CODE    13
ISR_ERROR_CODE    14
ISR_NO_ERROR_CODE 15
ISR_NO_ERROR_CODE 16
ISR_NO_ERROR_CODE 17
ISR_NO_ERROR_CODE 18
ISR_NO_ERROR_CODE 19
ISR_NO_ERROR_CODE 20
ISR_NO_ERROR_CODE 21
ISR_NO_ERROR_CODE 22
ISR_NO_ERROR_CODE 23
ISR_NO_ERROR_CODE 24
ISR_NO_ERROR_CODE 25
ISR_NO_ERROR_CODE 26
ISR_NO_ERROR_CODE 27
ISR_NO_ERROR_CODE 28
ISR_NO_ERROR_CODE 29
ISR_NO_ERROR_CODE 30
ISR_NO_ERROR_CODE 31

; IRQs
ISR_NO_ERROR_CODE 32
ISR_NO_ERROR_CODE 33
ISR_NO_ERROR_CODE 34
ISR_NO_ERROR_CODE 35
ISR_NO_ERROR_CODE 36
ISR_NO_ERROR_CODE 37
ISR_NO_ERROR_CODE 38
ISR_NO_ERROR_CODE 39
ISR_NO_ERROR_CODE 40
ISR_NO_ERROR_CODE 41
ISR_NO_ERROR_CODE 42
ISR_NO_ERROR_CODE 43
ISR_NO_ERROR_CODE 44
ISR_NO_ERROR_CODE 45
ISR_NO_ERROR_CODE 46
ISR_NO_ERROR_CODE 47

isr_common:
	save_context ; Save registers

	; Use the kernel data segment
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	mov rdi, rsp ;; Pass the stack to C handler
	call interrupt_handler
	restore_context ; Restore registers
	add rsp, 16 ; Pop error code & interrupt number
	iretq

global asm_switchTask
asm_switchTask:
	;; rdi: iretq stack
	;; rsi: task's pd
	mov rsp, rdi
	mov cr3, rsi

	restore_context ; Restore registers
	add rsp, 16 ; Pop error code & interrupt number
	iretq

section .data

;; An ISR stub table
;; Used to prevent excessive code reuse
global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    48
    dq isr%+i
%assign i i+1
%endrep




