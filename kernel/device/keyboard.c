#include "types.h"
#include "asm_util.h"
#include "keyboard.h"
#include "queue.h"
#include "synchronization.h"
#include "list.h"

void keyboard_handler() {
	BYTE tmp;

	if (is_output_buf_full() == TRUE) {
		tmp = get_keyboard_scancode();
		convert_scancode_and_inqueue(tmp);
	}
}

/****************************************************
 * the things relevant to controll keyboard device
 *
 * Keyboard -> Output Buffer -> CPU
 * Keyboard <- Input Buffer <- CPU
 ****************************************************/
BOOL is_output_buf_full(void) {
	if (in_port_b(0x64) & 0x01)
		return TRUE;
	return FALSE;
}

BOOL is_input_buf_full(void) {
	if (in_port_b(0x64) & 0x02)
		return TRUE;
	return FALSE;
}

// Wait ACK Scancode from Key Board
// - if the scancode is ACK, return TRUE
// - if not, inqueue the scancode and return FALSE
BOOL keyboard_wait_ack(void) {
	int i, j;
	BYTE data;
	BOOL result = FALSE;

	// read output buffer 100 times
	// if find  ACK, return TRUE
	for (j = 0; j < 100; j++) {
		for (i = 0; i < 0xFFFF; i++) {
			if (is_output_buf_full() == TRUE)
				break;
		}

		data = in_port_b(0x60);
		if (data == 0xFA) {
			result = TRUE;
			break;
		} else {
			convert_scancode_and_inqueue(data);
		}
	}
	return result;
}

BOOL activate_keyboard(void) {
	int i;
	BOOL pre_intr, result;

	pre_intr = set_intr(FALSE);

	out_port_b(0x64, 0xAE);

	for (i = 0; i < 0xFFFF; i++) {
		if (is_input_buf_full() == FALSE)
			break;
	}

	out_port_b(0x60, 0xF4);

	result = keyboard_wait_ack();

	set_intr(pre_intr);
	return result;
}

// read one byte from keyboard output buffer
BYTE get_keyboard_scancode(void) {
	while (is_output_buf_full() == FALSE)
		;
	return in_port_b(0x60);
}

BOOL change_keyboard_led(BOOL capslock_on, BOOL numlock_on, BOOL scrolllock_on) {
	int i, j;
	BOOL pre_intr, result, data;

	pre_intr = set_intr(FALSE);

	for (i = 0; i < 0xFFFF; i++) {
		if (is_input_buf_full() == FALSE)
			break;
	}

	out_port_b(0x60, 0xED);
	for (i = 0; i < 0xFFFF; i++) {
		if (is_input_buf_full() == FALSE) {
			break;
		}
	}

	result = keyboard_wait_ack();

	if (result == FALSE) {
		set_intr(pre_intr);
		return FALSE;
	}

	out_port_b(0x60, (capslock_on << 2) | (numlock_on << 1) | scrolllock_on);
	for (i = 0; i < 0xFFFF; i++) {
		if (is_input_buf_full() == FALSE)
			break;
	}

	result = keyboard_wait_ack();

	set_intr(pre_intr);
	return result;
}

/****************************************************
 * the things relevant to converting ScanCode to ASCII code
 ****************************************************/

keyboardmanager_t kbd_manager = { 0, };

static queue_t key_queue;
static key_data_t key_queue_buf[KEY_MAXQUEUE_COUNT];

// A table to convert Scan Code to ASCII Code
static key_mapping_entry_t key_mapping_table[KEY_MAPPINGTABLEMAXCOUNT] = {
/*  0   */{ KEY_NONE, KEY_NONE },
/*  1   */{ KEY_ESC, KEY_ESC },
/*  2   */{ '1', '!' },
/*  3   */{ '2', '@' },
/*  4   */{ '3', '#' },
/*  5   */{ '4', '$' },
/*  6   */{ '5', '%' },
/*  7   */{ '6', '^' },
/*  8   */{ '7', '&' },
/*  9   */{ '8', '*' },
/*  10  */{ '9', '(' },
/*  11  */{ '0', ')' },
/*  12  */{ '-', '_' },
/*  13  */{ '=', '+' },
/*  14  */{ KEY_BACKSPACE, KEY_BACKSPACE },
/*  15  */{ KEY_TAB, KEY_TAB },
/*  16  */{ 'q', 'Q' },
/*  17  */{ 'w', 'W' },
/*  18  */{ 'e', 'E' },
/*  19  */{ 'r', 'R' },
/*  20  */{ 't', 'T' },
/*  21  */{ 'y', 'Y' },
/*  22  */{ 'u', 'U' },
/*  23  */{ 'i', 'I' },
/*  24  */{ 'o', 'O' },
/*  25  */{ 'p', 'P' },
/*  26  */{ '[', '{' },
/*  27  */{ ']', '}' },
/*  28  */{ '\n', '\n' },
/*  29  */{ KEY_CTRL, KEY_CTRL },
/*  30  */{ 'a', 'A' },
/*  31  */{ 's', 'S' },
/*  32  */{ 'd', 'D' },
/*  33  */{ 'f', 'F' },
/*  34  */{ 'g', 'G' },
/*  35  */{ 'h', 'H' },
/*  36  */{ 'j', 'J' },
/*  37  */{ 'k', 'K' },
/*  38  */{ 'l', 'L' },
/*  39  */{ ';', ':' },
/*  40  */{ '\'', '\"' },
/*  41  */{ '`', '~' },
/*  42  */{ KEY_LSHIFT, KEY_LSHIFT },
/*  43  */{ '\\', '|' },
/*  44  */{ 'z', 'Z' },
/*  45  */{ 'x', 'X' },
/*  46  */{ 'c', 'C' },
/*  47  */{ 'v', 'V' },
/*  48  */{ 'b', 'B' },
/*  49  */{ 'n', 'N' },
/*  50  */{ 'm', 'M' },
/*  51  */{ ',', '<' },
/*  52  */{ '.', '>' },
/*  53  */{ '/', '?' },
/*  54  */{ KEY_RSHIFT, KEY_RSHIFT },
/*  55  */{ '*', '*' },
/*  56  */{ KEY_LALT, KEY_LALT },
/*  57  */{ ' ', ' ' },
/*  58  */{ KEY_CAPSLOCK, KEY_CAPSLOCK },
/*  59  */{ KEY_F1, KEY_F1 },
/*  60  */{ KEY_F2, KEY_F2 },
/*  61  */{ KEY_F3, KEY_F3 },
/*  62  */{ KEY_F4, KEY_F4 },
/*  63  */{ KEY_F5, KEY_F5 },
/*  64  */{ KEY_F6, KEY_F6 },
/*  65  */{ KEY_F7, KEY_F7 },
/*  66  */{ KEY_F8, KEY_F8 },
/*  67  */{ KEY_F9, KEY_F9 },
/*  68  */{ KEY_F10, KEY_F10 },
/*  69  */{ KEY_NUMLOCK, KEY_NUMLOCK },
/*  70  */{ KEY_SCROLLLOCK, KEY_SCROLLLOCK },

/*  71  */{ KEY_HOME, '7' },
/*  72  */{ KEY_UP, '8' },
/*  73  */{ KEY_PAGEUP, '9' },
/*  74  */{ '-', '-' },
/*  75  */{ KEY_LEFT, '4' },
/*  76  */{ KEY_CENTER, '5' },
/*  77  */{ KEY_RIGHT, '6' },
/*  78  */{ '+', '+' },
/*  79  */{ KEY_END, '1' },
/*  80  */{ KEY_DOWN, '2' },
/*  81  */{ KEY_PAGEDOWN, '3' },
/*  82  */{ KEY_INS, '0' },
/*  83  */{ KEY_DEL, '.' },
/*  84  */{ KEY_NONE, KEY_NONE },
/*  85  */{ KEY_NONE, KEY_NONE },
/*  86  */{ KEY_NONE, KEY_NONE },
/*  87  */{ KEY_F11, KEY_F11 },
/*  88  */{ KEY_F12, KEY_F12 } };

BOOL is_alphabet_scancode(BYTE key_scancode) {
	return (('a' <= key_mapping_table[key_scancode].normal_code)
			&& (key_mapping_table[key_scancode].normal_code <= 'z')) ?
			(TRUE) : (FALSE);
}

BOOL is_number_or_symbol_scancode(BYTE key_scancode) {
	return ((2 <= key_scancode) && (key_scancode <= 53)
			&& (is_alphabet_scancode(key_scancode) == FALSE)) ? (TRUE) : (FALSE);
}

BOOL is_number_pad_scancode(BYTE key_scancode) {
	return ((71 <= key_scancode) && (key_scancode <= 83)) ? (TRUE) : (FALSE);
}

BOOL is_use_combined_code(BYTE key_scancode) {
	BYTE down_scancode;
	BOOL use_combined_key = FALSE;

	down_scancode = key_scancode & 0x7F;

	// if scancode is Alphabet, shift and capslock are not matter
	if (is_alphabet_scancode(down_scancode) == TRUE) {
		if (kbd_manager.shift_down ^ kbd_manager.capslock_on) {
			use_combined_key = TRUE;
		} else {
			use_combined_key = FALSE;
		}
	}

	// if number or symbol, shift have affect
	else if (is_number_or_symbol_scancode(down_scancode) == TRUE) {
		if (kbd_manager.shift_down == TRUE) {
			use_combined_key = TRUE;
		} else {
			use_combined_key = FALSE;
		}
	}

	// if number pad key, numlock have affect
	else if ((is_number_pad_scancode(down_scancode) == TRUE)
			&& (kbd_manager.extended_code_in == FALSE)) {
		if (kbd_manager.numlock_on == TRUE) {
			use_combined_key = TRUE;
		} else {
			use_combined_key = FALSE;
		}
	}

	return use_combined_key;
}

void update_combination_key_status_and_led(BYTE key_scancode) {
	BOOL key_down, down_scancode, led_status_changed = FALSE;

	if (key_scancode & 0x80) {
		key_down = FALSE;
		down_scancode = key_scancode & 0x7F;
	} else {
		key_down = TRUE;
		down_scancode = key_scancode;
	}

	if ((down_scancode == 42) || (down_scancode == 54)) {
		kbd_manager.shift_down = key_down;
	} else if ((down_scancode == 58) && (key_down == TRUE)) {
		kbd_manager.capslock_on ^= TRUE;
		led_status_changed = TRUE;
	} else if ((down_scancode == 69) && (key_down == TRUE)) {
		kbd_manager.numlock_on ^= TRUE;
		led_status_changed = TRUE;
	} else if ((down_scancode == 70) && (key_down == TRUE)) {
		kbd_manager.scrolllock_on ^= TRUE;
		led_status_changed = TRUE;
	}

	if (led_status_changed == TRUE) {
		change_keyboard_led(kbd_manager.capslock_on, kbd_manager.numlock_on,
				kbd_manager.scrolllock_on);
	}
}

// Convert Scancode to ASCII code and Status flag
BOOL convert_scancode_to_asciicode_and_flag(BYTE key_scancode,
BYTE* pkey_ascii_code,
BOOL* pkey_flags) {
	BOOL use_combined_key;

	if (kbd_manager.skip_count_for_pause > 0) {
		kbd_manager.skip_count_for_pause--;
		return FALSE;
	}

	if (key_scancode == 0xE1) {
		*pkey_ascii_code = KEY_PAUSE;
		*pkey_flags = KEY_FLAGS_DOWN;
		kbd_manager.skip_count_for_pause = KEY_SKIPCOUNTFORPAUSE;
		return TRUE;
	} else if (key_scancode == 0xE0) {
		kbd_manager.extended_code_in = TRUE;
		return FALSE;
	}

	use_combined_key = is_use_combined_code(key_scancode);

	if (use_combined_key == TRUE) {
		*pkey_ascii_code = key_mapping_table[key_scancode & 0x7F].combined_code;
	} else {
		*pkey_ascii_code = key_mapping_table[key_scancode & 0x7F].normal_code;
	}

	if (kbd_manager.extended_code_in == TRUE) {
		*pkey_flags = KEY_FLAGS_EXTENDEDKEY;
		kbd_manager.extended_code_in = FALSE;
	} else {
		*pkey_flags = 0;
	}

	if ((key_scancode & 0x80) == 0) {
		*pkey_flags |= KEY_FLAGS_DOWN;
	}

	update_combination_key_status_and_led(key_scancode);
	return TRUE;
}

void keyboard_init(void) {
	queue_init(&key_queue, key_queue_buf, KEY_MAXQUEUE_COUNT,
			sizeof(key_data_t));

	spinlock_init(&(kbd_manager.spinlock));
	activate_keyboard();
	change_keyboard_led(FALSE, FALSE, FALSE);

	printk("keyboard, key-queue [ok]\n");
}

BOOL convert_scancode_and_inqueue(BYTE key_scancode) {
	key_data_t data;
	BOOL result = FALSE;

	data.key_scancode = key_scancode;

	if (convert_scancode_to_asciicode_and_flag(key_scancode,
			&(data.key_ascii_code), &(data.key_flags)) == TRUE) {
		spinlock_lock(&(kbd_manager.spinlock));
		result = inqueue(&key_queue, &data);
		spinlock_unlock(&(kbd_manager.spinlock));
	}

	return result;
}

BOOL get_key_from_key_queue(key_data_t* pstData) {
	BOOL result;

	spinlock_lock(&(kbd_manager.spinlock));
	result = dequeue(&key_queue, pstData);
	spinlock_unlock(&(kbd_manager.spinlock));

	return result;
}
