#include "util.h"
#include "asm_util.h"
#include "printk.h"
#include "dynamic.h"

extern volatile QWORD jiffies;

void print_regs(regs_t *r) {
	printk("ss:%x rsp:%x rflags:%x\n", r->ss, r->rsp, r->rflags);
	printk("cs:%x rip:%x\n", r->cs, r->rip);
	printk("err_code:%x int_no:%x\n", r->err_code, r->int_no);
	printk("rbp:%x rax:%x rbx:%x\n", r->rbp, r->rax, r->rbx);
	printk("rcx:%x rdx:%x rdi:%x\n", r->rcx, r->rdx, r->rdi);
	printk("rsi:%x r8:%x r9:%x\n", r->rsi, r->r8, r->r9);
	printk("r10:%x r11:%x r12:%x\n", r->r10, r->r11, r->r12);
	printk("r13:%x r14:%x r15:%x\n", r->r13, r->r14, r->r15);
	printk("ds:%x es:%x fs:%x\n", r->ds, r->es, r->fs);
	printk("gs:%x\n", r->gs);
}

/*
 * inportb
 * Read from an I/O port.
 */
unsigned char in_port_b(unsigned short _port) {
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	// out dx, al
	// dx = port num
	// al = data register
	return rv;
}

/*
 * outportb
 * Write to an I/O port.
 */
void out_port_b(unsigned short _port, unsigned char _data) {
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
	// in al, dx
	// dx = portnum
	// al = data register
}

void* memset(void * b, int val, size_t count) {
	__asm__ __volatile__ ("cld; rep stosb" : "+c" (count), "+D" (b) : "a" (val) : "memory");
	return b;
}

void* memcpy(void * dest, const void * src, size_t count) {
	__asm__ __volatile__ ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dest) :: "memory");
	return dest;
}

int memcmp(const char * a, const char * b, const int size) {
	DWORD i = 0;
	while (i < size) {
		if (a[i] < b[i]) {
			return -1;
		} else if (a[i] > b[i]) {
			return 1;
		}
		i++;
	}
	return 0;
}

char* strdup(const char *str) {
	int len = strlen(str);
	char * out = kmalloc(sizeof(char) * (len + 1));
	memcpy(out, str, len + 1);
	return out;
}

int strcat(void* dest, const void* src) {
	int i, src_size, dest_size;

	for (i = 0; ((BYTE*) dest)[i] != NULL; i++)
		;
	dest_size = i;

	for (i = 0; ((BYTE*) src)[i] != NULL; i++)
		;
	src_size = i;

	memcpy(dest + dest_size, src, src_size + 1);

	return dest_size + src_size;
}

char * strcpy(char * dest, const char * src) {
	int len = strlen(src);
	memcpy(dest, src, len + 1);
	return dest;
}

BOOL set_intr(BOOL flag) {
	QWORD rflags;

	rflags = read_rflags();
	if (flag == TRUE) {
		enable_intr();
	} else {
		disable_intr();
	}

	if (rflags & 0x0200) {
		return TRUE;
	}
	return FALSE;
}

int strlen(const char *str) {
	int i = 0;

	while (str[i] != (char) 0) {
		++i;
	}

	return i;
}

QWORD get_memsize(void) {
	//return 128 * 1024; // 128 mb
	return 128;
}

QWORD atoi(const char* str) {
	QWORD len = strlen(str);
	QWORD out = 0;
	QWORD i;
	QWORD pow = 1;
	for (i = len; i > 0; --i) {
		out += (str[i - 1] - 48) * pow;
		pow *= 10;
	}
	return out;
}

int itoa(long value, char* buf, int radix) {
	QWORD out;

	switch (radix) {
	case 16:
		print_hex(value, 0, buf, &out);
		return out;

	case 10:
	default:
		print_dec(value, 0, buf, &out);
		return out;
	}
}

void str_reverse(char* buf) {
	int len, i, tmp;

	len = strlen(buf);
	for (i = 0; i < len / 2; i++) {
		tmp = buf[i];
		buf[i] = buf[len - 1 - i];
		buf[len - 1 - i] = tmp;
	}
}

void sleep(QWORD millisec) {
	QWORD prev_jiffies;

	prev_jiffies = jiffies;

	while ((jiffies - prev_jiffies) <= millisec) {
		schedule();
	}
}
