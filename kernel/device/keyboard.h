#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "types.h"
#include "synchronization.h"
#include "fault.h"
#include "sys.h"

// the number of scan code which will be ignored to process PAUSE key
#define KEY_SKIPCOUNTFORPAUSE       2

// Key status flag
#define KEY_FLAGS_UP             0x00
#define KEY_FLAGS_DOWN           0x01
#define KEY_FLAGS_EXTENDEDKEY    0x02

// scancode mapping table
#define KEY_MAPPINGTABLEMAXCOUNT    89

#define KEY_NONE        0x00
#define KEY_ENTER       '\n'
#define KEY_TAB         '\t'
#define KEY_ESC         0x1B
#define KEY_SPACE       0x20
#define KEY_BACKSPACE   0x08

#define KEY_CTRL        0x81
#define KEY_LSHIFT      0x82
#define KEY_RSHIFT      0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT        0x85
#define KEY_CAPSLOCK    0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_NUMLOCK     0x91
#define KEY_SCROLLLOCK  0x92
#define KEY_HOME        0x93
#define KEY_UP          0x94
#define KEY_PAGEUP      0x95
#define KEY_LEFT        0x96
#define KEY_CENTER      0x97
#define KEY_RIGHT       0x98
#define KEY_END         0x99
#define KEY_DOWN        0x9A
#define KEY_PAGEDOWN    0x9B
#define KEY_INS         0x9C
#define KEY_DEL         0x9D
#define KEY_F11         0x9E
#define KEY_F12         0x9F
#define KEY_PAUSE       0xA0

// Max key que size
#define KEY_MAXQUEUE_COUNT   100

#pragma pack( push, 1 )
typedef struct {
	// the ascii code which is not combined with SHIFT or CAPS LOCK
	BYTE normal_code;

	// the ascii code which is combined with SHIFT or CAPS LOCK
	BYTE combined_code;
} key_mapping_entry_t;

typedef struct {
	spinlock_t spinlock;

	// Combined Key Information
	BOOL shift_down;
	BOOL capslock_on;
	BOOL numlock_on;
	BOOL scrolllock_on;

	// Informations to process Extended key
	BOOL extended_code_in;
	int skip_count_for_pause;
} keyboardmanager_t;

// the data type which is used as KEY QUEUE
typedef struct {
	BYTE key_scancode;
	BYTE key_ascii_code;
	// key status flag
	// 3 status : KEY_DOWN, KEY_UP, EXTENDED_KEY
	BYTE key_flags;
} key_data_t;
#pragma pack( pop )

void keyboard_handler();

BOOL is_output_buf_full(void);
BOOL is_input_buf_full(void);
BOOL activate_keyboard(void);
BYTE get_keyboard_scancode(void);
BOOL change_keyboard_led(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn);

BOOL is_alphabet_scancode(BYTE key_scancode);
BOOL is_number_or_symbol_scancode(BYTE key_scancode);
BOOL is_number_pad_scancode(BYTE key_scancode);
BOOL is_use_combined_code(BYTE key_scancode);
void update_combination_key_status_and_led(BYTE key_scancode);
BOOL convert_scancode_to_asciicode_and_flag(BYTE key_scancode,
		BYTE* pkey_ascii_code,
		BOOL* pkey_flags);
void keyboard_init(void);
BOOL convert_scancode_and_inqueue(BYTE key_scancode);
BOOL get_key_from_key_queue(key_data_t* pstData);
BOOL keyboard_wait_ack(void);

#endif /* KEYBOARD_H_ */

