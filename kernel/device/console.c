#include <stdarg.h>
#include "console.h"
#include "keyboard.h"

void console_init() {
	int i;

	// dbg console
	vga_console.dbg_screen = DBG_CONSOLE_PHYSMEM_ADDR + PAGE_OFFSET;

	for (i = 0; i < CONSOLE_WIDTH * DBG_CONSOLE_HEIGHT; i++)
		vga_console.dbg_screen[i].attr = DBG_CONSOLE_DEFAULT_ATTR;

	// normal console
	vga_console.screen = CONSOLE_PHYSMEM_ADDR + PAGE_OFFSET;

	for (i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++)
		vga_console.screen[i].attr = CONSOLE_DEFAULT_ATTR;

	set_cursor(0, 0);
}

void set_cursor(int x, int y) {
	int linear_offset = y * CONSOLE_WIDTH + x;

	out_port_b(VGA_PORT_INDEX, VGA_INDEX_UPPERCURSOR);
	out_port_b(VGA_PORT_DATA, linear_offset >> 8);

	out_port_b(VGA_PORT_INDEX, VGA_INDEX_LOWERCURSOR);
	out_port_b(VGA_PORT_DATA, linear_offset & 0xFF);

	vga_console.curr_offset = linear_offset;
}

void get_cursor(int *x, int *y) {
	*x = vga_console.curr_offset % CONSOLE_WIDTH;
	*y = vga_console.curr_offset / CONSOLE_WIDTH;
}

// Print string to console and Update next_print_offset
int console_print_str(const char* buf) {
	int i, j;
	int len = strlen(buf);
	int offset = vga_console.curr_offset;
	struct console_char *screen = vga_console.screen;

	for (i = 0; i < len; i++) {
		if (buf[i] == '\n') {
			offset += (CONSOLE_WIDTH - (offset % CONSOLE_WIDTH));
		} else if (buf[i] == '\b') {
			offset--;
			screen[offset].letter = ' ';
		} else if (buf[i] == '\t') {
			offset += (8 - (offset % 8));
		} else {
			screen[offset].letter = buf[i];
			offset++;
		}

		// if console is full, scroll up one line
		if (offset >= (CONSOLE_HEIGHT * CONSOLE_WIDTH)) {
			// move one line up except upper line
			memcpy(screen,
					(BYTE*) screen
							+ CONSOLE_WIDTH * sizeof(struct console_char),
					(CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH
							* sizeof(struct console_char));

			// fill up bottom line with ' '
			for (j = (CONSOLE_HEIGHT - 1) * (CONSOLE_WIDTH);
					j < (CONSOLE_HEIGHT * CONSOLE_WIDTH); j++) {
				screen[j].letter = ' ';
			}

			// reset next_print_offset to bottom line's head
			offset = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH;
		}
	}
	return offset;
}

// clear screen
void clear_console(void) {
	int i;

	for (i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; i++) {
		vga_console.screen[i].letter = ' ';
	}

	set_cursor(0, 0);
}

BYTE getch(void) {
	key_data_t data;

	// wait until key pushed
	while (1) {
		while (get_key_from_key_queue(&data) == FALSE)
			schedule();

		if (data.key_flags & KEY_FLAGS_DOWN)
			return data.key_ascii_code;
	}
}

void dbg_console_printxy(int x, int y, const char* str) {
	int i = 0;
	struct console_char *screen = vga_console.dbg_screen + (y * CONSOLE_WIDTH)
			+ x;
	while (str[i]) {
		screen[i].letter = str[i];
		i++;
	}
}

