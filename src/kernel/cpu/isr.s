	EXTERN common_intr_handler

%macro ISR_NOERR 1
	GLOBAL _isr%1
_isr%1:
	push byte 0
	push byte %1
	jmp common_intr_entry
%endmacro

%macro ISR_ERR 1
	GLOBAL _isr%1
_isr%1:
	push byte %1
	jmp common_intr_entry
%endmacro

; CPU FAULT HANDLER
	ISR_NOERR 0
	ISR_NOERR 1
	ISR_NOERR 2
	ISR_NOERR 3
	ISR_NOERR 4
	ISR_NOERR 5
	ISR_NOERR 6
	ISR_NOERR 7
	ISR_ERR   8
	ISR_NOERR 9
	ISR_ERR   10
	ISR_ERR   11
	ISR_ERR   12
	ISR_ERR   13
	ISR_ERR   14
	ISR_NOERR 15
	ISR_NOERR 16
	ISR_NOERR 17
	ISR_NOERR 18
	ISR_NOERR 19
	ISR_NOERR 20
	ISR_NOERR 21
	ISR_NOERR 22
	ISR_NOERR 23
	ISR_NOERR 24
	ISR_NOERR 25
	ISR_NOERR 26
	ISR_NOERR 27
	ISR_NOERR 28
	ISR_NOERR 29
	ISR_NOERR 30
	ISR_NOERR 31

; DEVICE ITERRUPT HANDLER
	ISR_NOERR 32
	ISR_NOERR 33
	ISR_NOERR 34
	ISR_NOERR 35
	ISR_NOERR 36
	ISR_NOERR 37
	ISR_NOERR 38
	ISR_NOERR 39
	ISR_NOERR 40
	ISR_NOERR 41
	ISR_NOERR 42
	ISR_NOERR 43
	ISR_NOERR 44
	ISR_NOERR 45
	ISR_NOERR 46
	ISR_NOERR 47

; SYSTEM CALL HANDLER
	ISR_NOERR 99

common_intr_entry:
	push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

	mov ax, ds
    push rax
    mov ax, es
    push rax
    push fs
    push gs

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov rdi, rsp
	call common_intr_handler
	; ...
	; push eip
	; jmp
	; push ebp
	; mov ebp, esp
	; sub esp, 16
	; ...
	pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
    add rsp, 16
	iretq

