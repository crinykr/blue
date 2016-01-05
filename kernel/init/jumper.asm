; OSBLUE AP Start Point

; start at 0x10000 in 16bit mode by AP

	[org 0x10000]
	[bits 16]
	mov ax, 0x1000
	mov ds, ax
	; IF bit clearing is required when changing mode 32
	lgdt [ EARLYGDTR ]

	mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
	mov cr0, eax
	jmp dword 0x8:start32

	[bits 32]
start32:
	mov ax, 0x10
	mov ds, ax
	jmp 0x10008e ; jump to prepare64

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; early GDT area
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	align 8, db 0
	dw 0x0000
EARLYGDTR:
    dw GDTEND - GDT - 1
    dd GDT
GDT:
	NULLDescriptor:
		dq 0
	CODEDESCRIPTOR:
		dw 0xFFFF       ; Limit [15:0]
		dw 0x0000       ; Base [15:0]
		db 0x00         ; Base [23:16]
		db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
		db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
		db 0x00         ; Base [31:24]
	DATADESCRIPTOR:
		dw 0xFFFF       ; Limit [15:0]
		dw 0x0000       ; Base [15:0]
		db 0x00         ; Base [23:16]
		db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
		db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
		db 0x00         ; Base [31:24]
GDTEND:
