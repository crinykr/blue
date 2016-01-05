[BITS 64]
SECTION .text

global load_cr3
global load_gdtr, load_tr, load_idtr
global enable_intr, disable_intr, read_rflags
global switch_context, hlt, test_and_set, pause
global x86_finit, save_fpu_context, load_fpu_context, set_ts, clear_ts
global enable_global_lapic, write_msr, lock_inc_dword

; C-Standard 64bit parameter rules
; RDI(param 1), RSI(param 2), ...

load_cr3:
	push rax

	mov rax, rdi
	mov cr3, rax

	pop rax
	ret


;lock inc dword [rel ap_on_count ]
lock_inc_dword:
	lock inc dword [rdi]
	ret

load_gdtr:
    lgdt [ rdi ]
    ret

load_tr:
    ltr di
    ret

load_idtr:
    lidt [ rdi ]
    ret

enable_intr:
    sti
    ret

disable_intr:
    cli
    ret

read_rflags:
    pushfq
    pop rax
    ret

write_msr:
    push rdx
    push rax
    push rcx

    mov rcx, rdi ; set msr address
    mov rax, rdx ; lower 32bit
    mov rdx, rsi ; upper 32bit
    wrmsr

    pop rcx
    pop rax
    pop rdx
    ret

%macro SAVECONTEXT 0
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
%endmacro

%macro LOADCONTEXT 0
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
%endmacro

; PARAM1(RDI): Current Context, PARAM2(RSI): Next Context
switch_context:
    push rbp
    mov rbp, rsp

    ; if PARAM1 is NULL, no need to save current context
    pushfq
    cmp rdi, 0
    je .LoadContext
    popfq

    ; [1] Save current task context to task struct
    push rax ; save register to be used temporaily

    ; save SS, RSP, RFLAGS, CS, RIP
    mov ax, ss
    mov qword[ rdi + ( 25 * 8 ) ], rax ; save ss

    mov rax, rbp
    add rax, 16
    mov qword[ rdi + ( 24 * 8 ) ], rax ; save esp

    pushfq
    pop rax
    mov qword[ rdi + ( 23 * 8 ) ], rax ; save rflags

    mov ax, cs
    mov qword[ rdi + ( 22 * 8 ) ], rax ; save cs

    mov rax, qword[ rbp + 8 ]
    mov qword[ rdi + ( 21 * 8 ) ], rax ; save rip. rip is next of address which call switch_context

    pop rax
    pop rbp

    add rdi, ( 19 * 8 )
    mov rsp, rdi
    sub rdi, ( 19 * 8 )

    SAVECONTEXT

    ; [2] Save next task context to kernel stack
.LoadContext:
    mov rsp, rsi

    LOADCONTEXT
    add rsp, 16
    iretq



hlt:
;    pause
;	hlt
    ret



; Test and Mov at same time
; Mov "Source(RAX)" to "memory[Deenable_intrnation(RDI)]" if "memory[Deenable_intrnation(RDI)]" equals to  "Compare(RAX)"
;   PARAM1(rdi): Deenable_intrnation
;   PRAAM2(rsi): Compare
;	PARAM3(rdx): Source
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; lock cmpxchg byte [ rdi ], dl
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; [1] compare memory[Deenable_intrnation(RDI)] and Compare(RAX)
; cmp byte[rdi], rax
; jne fail
; [2] mov Souce to memory[Deenable_intrnation(RDI)]
; mov byte[rdi], dl
; set ZF
; return
;fail:
; clear ZF
; return
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
test_and_set:
    mov rax, rsi
    lock cmpxchg byte [ rdi ], dl
    je .SUCCESS; if ZF is 1, it means two values are same. So jump to SUCCESS

.NOTSAME: ; Deenable_intrnation not equals to Compare
    mov rax, 0x00
    ret

.SUCCESS: ; Deenable_intrnation equals to Comparea
    mov rax, 0x01
    ret

x86_finit:
    finit
    ret

save_fpu_context:
    fxsave  [ rdi ]
    ret

load_fpu_context:
    fxrstor [ rdi ]
    ret

; Set TS bit in CR0
set_ts:
    push rax

    mov rax, cr0
    or rax, 0x08
    mov cr0, rax

    pop rax
    ret

clear_ts:
    clts
    ret

; Enable All LOCAL-APIC
; set GLOBAL-APIC-BIT(bit 11) in IA32_APIC_BASE MSR
enable_global_lapic:
    push rax
    push rcx
    push rdx

    mov rcx, 27 ; set IA32_APIC_BASE_MSR register number
    rdmsr ; EDX(upper 32bit), EAX(lower 32bit)

    or eax, 0x0800
    wrmsr

    pop rdx
    pop rcx
    pop rax
    ret

pause:
	mov rax, 0x12341234 ; pause magic num
    pause
    ret





