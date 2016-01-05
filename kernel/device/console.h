#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "types.h"

#define CONSOLE_BACKGROUND_DARKBLACK        0x00
#define CONSOLE_BACKGROUND_DARKBLUE         0x10
#define CONSOLE_BACKGROUND_DARKGREEN        0x20
#define CONSOLE_BACKGROUND_DARKCYAN         0x30
#define CONSOLE_BACKGROUND_DARKRED          0x40
#define CONSOLE_BACKGROUND_DARKMAGENTA      0x50
#define CONSOLE_BACKGROUND_DARKBROWN        0x60
#define CONSOLE_BACKGROUND_DARKWHITE        0x70

#define CONSOLE_BACKGROUND_BRIGHTBLACK      0x80
#define CONSOLE_BACKGROUND_BRIGHTBLUE       0x90
#define CONSOLE_BACKGROUND_BRIGHTGREEN      0xA0
#define CONSOLE_BACKGROUND_BRIGHTCYAN       0xB0
#define CONSOLE_BACKGROUND_BRIGHTRED        0xC0
#define CONSOLE_BACKGROUND_BRIGHTMAGENTA    0xD0
#define CONSOLE_BACKGROUND_BRIGHTBROWN      0xE0
#define CONSOLE_BACKGROUND_BRIGHTWHITE      0xF0

#define CONSOLE_BACKGROUND_BLINK            0x80

#define CONSOLE_FOREGROUND_DARKBLACK        0x00
#define CONSOLE_FOREGROUND_DARKBLUE         0x01
#define CONSOLE_FOREGROUND_DARKGREEN        0x02
#define CONSOLE_FOREGROUND_DARKCYAN         0x03
#define CONSOLE_FOREGROUND_DARKRED          0x04
#define CONSOLE_FOREGROUND_DARKMAGENTA      0x05
#define CONSOLE_FOREGROUND_DARKBROWN        0x06
#define CONSOLE_FOREGROUND_DARKWHITE        0x07

#define CONSOLE_FOREGROUND_BRIGHTBLACK      0x08
#define CONSOLE_FOREGROUND_BRIGHTBLUE       0x09
#define CONSOLE_FOREGROUND_BRIGHTGREEN      0x0A
#define CONSOLE_FOREGROUND_BRIGHTCYAN       0x0B
#define CONSOLE_FOREGROUND_BRIGHTRED        0x0C
#define CONSOLE_FOREGROUND_BRIGHTMAGENTA    0x0D
#define CONSOLE_FOREGROUND_BRIGHTYELLOW     0x0E
#define CONSOLE_FOREGROUND_BRIGHTWHITE      0x0F

#define DBG_CONSOLE_DEFAULT_ATTR ( CONSOLE_BACKGROUND_DARKBLUE | CONSOLE_FOREGROUND_BRIGHTWHITE )
#define CONSOLE_DEFAULT_ATTR ( CONSOLE_BACKGROUND_DARKWHITE | CONSOLE_FOREGROUND_DARKBLUE )

// console top 2 line is used for debug
#define CONSOLE_WIDTH 80
#define DBG_CONSOLE_HEIGHT 2
#define CONSOLE_HEIGHT 23

// VGA TEXT MODE PORT
#define VGA_PORT_INDEX              0x3D4
#define VGA_PORT_DATA               0x3D5
#define VGA_INDEX_UPPERCURSOR       0x0E
#define VGA_INDEX_LOWERCURSOR       0x0F

struct {
	int curr_offset;
	struct console_char *screen;
	struct console_char *dbg_screen;
} vga_console;

#pragma pack( push, 1 )
struct console_char {
	BYTE letter;
	BYTE attr;
};
#pragma pack( pop )

#define DBG_CONSOLE_PHYSMEM_ADDR 0xb8000#define CONSOLE_PHYSMEM_ADDR (0xb8000 + CONSOLE_WIDTH * DBG_CONSOLE_HEIGHT * sizeof(struct console_char))

void console_init();
void set_cursor(int x, int ys);
void get_cursor(int *x, int *y);
int console_print_str(const char* buf);
void clear_console(void);
BYTE getch(void);
void dbg_console_printxy(int x, int y, const char* str);

#endif /* CONSOLE_H_ */
