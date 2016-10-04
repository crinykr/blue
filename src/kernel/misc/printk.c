/*
 * printk.c
 *
 *  Created on: 2014. 8. 24.
 *      Author: criny
 */

#include "printk.h"
#include "console.h"
#include "synchronization.h"

/*
 * Integer to string
 */
void print_dec(QWORD value, QWORD width, char * buf,
QWORD * ptr) {
	QWORD n_width = 1;
	QWORD i = 9;

	while (value > i && i < UINT64_MAX) {
		n_width += 1;
		i *= 10;
		i += 9;
	}

	int printed = 0;
	while (n_width + printed < width) {
		buf[*ptr] = '0';
		*ptr += 1;
		printed += 1;
	}

	i = n_width;
	while (i > 0) {
		QWORD n = value / 10;
		QWORD r = value % 10;
		buf[*ptr + i - 1] = r + '0';
		i--;
		value = n;
	}
	*ptr += n_width;
}

/*
 * Hexadecimal to string
 */
void print_hex(QWORD value, QWORD width, char * buf,
QWORD * ptr) {
	QWORD i = width;

	if (i == 0)
		i = 16;

	QWORD n_width = 1;
	QWORD j = 0x0F;
	while (value > j && j < UINT64_MAX) {
		n_width += 1;
		j *= 0x10;
		j += 0x0F;
	}

	while (i > (QWORD) n_width) {
		buf[*ptr] = '0';
		*ptr += 1;
		i--;
	}

	i = (QWORD) n_width;
	while (i-- > 0) {
		buf[*ptr] = "0123456789abcdef"[(value >> (i * 4)) & 0xF];
		*ptr += +1;
	}
}

/*
 * vasprintf()
 */
size_t vasprintf(char * buf, const char *fmt, va_list args) {
	QWORD i = 0;
	char *s;
	QWORD ptr = 0;
	QWORD len = strlen(fmt);
	for (; i < len && fmt[i]; ++i) {
		if (fmt[i] != '%') {
			buf[ptr++] = fmt[i];
			continue;
		}
		++i;
		QWORD arg_width = 0;
		while (fmt[i] >= '0' && fmt[i] <= '9') {
			arg_width *= 10;
			arg_width += fmt[i] - '0';
			++i;
		}
		/* fmt[i] == '%' */
		switch (fmt[i]) {
		case 's': /* String pointer -> String */
			s = (char *) va_arg(args, char *);
			while (*s) {
				buf[ptr++] = *s++;
			}
			break;
		case 'c': /* Single character */
			buf[ptr++] = (char) va_arg(args, int);
			break;
		case 'x': /* Hexadecimal number */
			print_hex((unsigned long) va_arg(args, unsigned long), arg_width,
					buf, &ptr);
			break;
		case 'd': /* Decimal number */
			print_dec((QWORD) va_arg(args, QWORD), arg_width, buf, &ptr);
			break;
		case '%': /* Escape */
			buf[ptr++] = '%';
			break;
		default: /* Nothing at all, just dump it */
			buf[ptr++] = fmt[i];
			break;
		}
	}
	/* Ensure the buffer ends in a null */
	buf[ptr] = '\0';
	return ptr;
}

/*
 * (Kernel) Print a formatted string.
 * %s, %c, %x, %d, %%
 *
 * @param fmt Formatted string to print
 * @param ... Additional arguments to format
 */

static spinlock_t spinlock_printk;

void printk(const char *str, ...) {
	va_list ap;
	char buf[1024];
	int next_offset;

	va_start(ap, str);
	vasprintf(buf, str, ap);
	va_end(ap);

	spinlock_lock(&spinlock_printk); // lock
	next_offset = console_print_str(buf);
	set_cursor(next_offset % CONSOLE_WIDTH, next_offset / CONSOLE_WIDTH);
	spinlock_unlock(&spinlock_printk); // unlock
}

int sprintf(char * buf, const char *fmt, ...) {
	va_list args;
	int out;

	va_start(args, fmt);
	out = vasprintf(buf, fmt, args);
	va_end(args);

	return out;
}

