;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OSBLUE is released under the terms
; of the NCSA / University of Illinois License
; Copyright (C) 2014-2015 criny
;
; OSBLUE is based on linux, minix, ToAruOS, Mint64OS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; OSBLUE BP Start Point

; Start at 0x100000 in 32bit mode by "Grub 32bit hook mode"

	EXTERN main_bp, main_ap, ap_on_flag
	EXTERN lapic_vaddr, ap_on_count

	ORG equ 0x100000
	PAGE_OFFSET equ 0xffff800000000000

SECTION .text.header

[bits 32]
start32:
	jmp 0x10:_start32 - start32 + ORG
_start32:
	mov eax, 0x18
	mov ds, ax

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Early 8GB page mapping table
	; vmem                                 phys                                 desc
	; 0000000000000000 - 0000000040000000  0000000000000000 - 0000000040000000  just fot mode changing (purged after all AP booting completed)
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	PAGE_FLAGS_P  equ 0x00000001
	PAGE_FLAGS_RW equ 0x00000002
	PAGE_FLAGS_US equ 0x00000004
	PAGE_FLAGS_PS equ 0x00000080
	PAGE_FLAGS_DEFAULT	equ ( PAGE_FLAGS_P | PAGE_FLAGS_RW )
	PGT_START equ ( EARLY_PGT - start32 + ORG )

	; [1] PML4 table
	; entry 0
	mov dword[PGT_START + 0 * 8], (PGT_START + 0x1000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	; entry 256
	mov dword[PGT_START + 256 * 8], (PGT_START + 0x1000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)

	; [2] Page directory pointer table
	; entry 0 ~ 7
	mov dword[PGT_START + 0x1000 + 0 * 8], (PGT_START + 0x2000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 1 * 8], (PGT_START + 0x3000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 2 * 8], (PGT_START + 0x4000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 3 * 8], (PGT_START + 0x5000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 4 * 8], (PGT_START + 0x6000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 5 * 8], (PGT_START + 0x7000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 6 * 8], (PGT_START + 0x8000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)
	mov dword[PGT_START + 0x1000 + 7 * 8], (PGT_START + 0x9000 | PAGE_FLAGS_DEFAULT | PAGE_FLAGS_US)

	; [3] Page directory table : (entry 0 ~ 511) x 8
	mov ecx, 512 * 8
	mov edi, PGT_START + 0x2000
	mov eax, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS
loop:
	mov [edi], eax
	add edi, 8
	add eax, 0x200000
	dec ecx
	jnz loop

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; Prepare for enabling 64bit mode
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
prepare64: ; here needs fixed for AP
	; set PAE(bit 5), OSXMMEXCPT(bit 10), OSFXSR(bit 9)
	mov eax, cr4
	or eax, 0x620
	mov cr4, eax

	; set PML4 start address
	mov eax, PGT_START
	mov cr3, eax

	; set LME(bit 8), SCE(bit 0)
	mov ecx, 0xC0000080 ; ecx = IA32_EFER MSR reg number
	rdmsr
	or eax, 0x0101 ; eax = IA32_EFER MSR lower 32bit
	wrmsr

    ; enable cache and paging : NW(bit 29) = 0, CD(bit 30) = 0, PG(bit 31) = 1
    ; enbale FPU : TS(bit 3) = 1, EM(bit 2) = 0, MP(bit 1) = 1
	mov eax, cr0
	or eax, 0xE000000E
	xor eax, 0x60000004
	mov cr0, eax

	lgdt [EARLYGDTR - start32 + ORG]
	jmp 0x8:start64 - start32 + ORG

[bits 64]
start64:
	mov rax, _start64
	jmp rax
_start64:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

    cmp byte [rel ap_on_flag ], 0x01 ; if AP
    je ap_stack_setting

bp_stack_setting:
	mov rsp, STACK_CORE0 + 1024 * 8 - 1
	mov rbp, STACK_CORE0 + 1024 * 8 - 1

	jmp main_bp

ap_stack_setting:
	xor rax, rax
    mov rbx, [rel lapic_vaddr ] ; read APIC ID register

    mov eax, dword [ rbx ] ; APIC ID (bit 24~31)
    shr rax, 24

    mov rbx, 1024 * 8
    mul rbx ; rax = rax(APIC ID : 1 ~ 3) x rbx(stack size)

	mov rsp, STACK_CORE0 + 1024 * 8 - 1
	mov rbp, STACK_CORE0 + 1024 * 8 - 1

    add rsp, rax
    add rbp, rax

	;lock inc dword [rel ap_on_count ]

	jmp main_ap

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Early GDT area
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 8, db 0
	dw 0x0000

EARLYGDTR:
	dw GDTEND - GDT - 1 ; gdt size
	dd GDT - start32 + ORG ; gdt start address

GDT:
	NULLDescriptor:
		dq 0

	; early 64bit code
	CODEDESCRIPTOR64:
		dw 0xFFFF       ; Limit [15:0]
		dw 0x0000       ; Base [15:0]
		db 0x00         ; Base [23:16]
		db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
		db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
		db 0x00         ; Base [31:24]
        
	; early 64bit data
	DATADESCRIPTOR64:
		dw 0xFFFF       ; Limit [15:0]
		dw 0x0000       ; Base [15:0]
		db 0x00         ; Base [23:16]
		db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
		db 0xAF         ; G=1, D=0, L=1, Limit[19:16]
		db 0x00         ; Base [31:24]
GDTEND:


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Early PGT area
; pml4 * 1 + pdpt * 1 + pd * 8 = 4096 * 8 Byte
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 4096, db 0
	GLOBAL EARLY_PGT
EARLY_PGT:
	resb 4096 * 10


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Kernel stack for Idle Process : Each 8kb size
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LOCAL APIC ID 0 or 0x10(lenovo)
	GLOBAL STACK_CORE0
STACK_CORE0: ; this is BP
	resb 1024 * 8

; LOCAL APIC ID 1
STACK_CORE1:
	resb 1024 * 8

; LOCAL APIC ID 2
STACK_CORE2:
	resb 1024 * 8

; LOCAL APIC ID 3
STACK_CORE3:
	resb 1024 * 8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; IST 8kb x 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 16, db 0
	GLOBAL IST_CORE0
IST_CORE0:
	resb 1024 * 8
IST_CORE1:
	resb 1024 * 8
IST_CORE2:
	resb 1024 * 8
IST_CORE3:
	resb 1024 * 8
dummy:
	resb 0xff
