//    preprocess        compile        assemble        linking
//*.c ----------> *.i ----------> *.s ----------> *.o ----------> *.out

#include "builtinshell.h"
#include "console.h"
#include "keyboard.h"
#include "util.h"
#include "pit.h"
#include "asm_util.h"
#include "task.h"
#include "synchronization.h"
#include "local_apic.h"
#include "multiprocessor.h"
#include "io_apic.h"
#include "fault.h"
#include "syscall.h"
#include "list.h"
#include "scheduler.h"
#include "printk.h"

extern volatile QWORD jiffies;

static cmd_entry_t cmd_table[] = {

{ "ps", cmd_ps }

};

void builtin_shell(void) {
	char cmd_buf[SHELL_CMDBUF_MAX];
	int cmd_buf_index = 0;
	BYTE key;

	printk("\n" SHELL_PROMPT);

	while (1) {
		switch (key = getch()) {
		case KEY_LSHIFT:
		case KEY_RSHIFT:
		case KEY_CAPSLOCK:
		case KEY_NUMLOCK:
		case KEY_SCROLLLOCK:
			break;

		case KEY_BACKSPACE:
			if (cmd_buf_index > 0) {
				printk("\b");
				cmd_buf_index--;
			}

			break;
		case KEY_ENTER:
			printk("\n");

			if (cmd_buf_index > 0) {
				cmd_buf[cmd_buf_index] = '\0';
				run_cmd(cmd_buf);
			}

			printk("%s", SHELL_PROMPT);
			memset(cmd_buf, '\0', SHELL_CMDBUF_MAX);
			cmd_buf_index = 0;

			break;
		default:
			if (key == KEY_TAB) {
				key = ' ';
			}

			if (cmd_buf_index < SHELL_CMDBUF_MAX) {
				cmd_buf[cmd_buf_index++] = key;
				printk("%c", key);
			}
		}
	}
}

static void run_cmd(const char* cmd_buf) {
	int i, space_index;
	int cmd_buf_len, cmd_len;
	int count;

	// trim and parsing
	cmd_buf_len = strlen(cmd_buf);
	for (space_index = 0; space_index < cmd_buf_len; space_index++) {
		if (cmd_buf[space_index] == ' ') {
			break;
		}
	}

	// search cmd in cmd_table
	count = sizeof(cmd_table) / sizeof(cmd_entry_t);
	for (i = 0; i < count; i++) {
		cmd_len = strlen(cmd_table[i].cmd);
		if ((cmd_len == space_index)
				&& (memcmp(cmd_table[i].cmd, cmd_buf, space_index) == 0)) {
			cmd_table[i].handler(cmd_buf + space_index + 1);
			break;
		}
	}

	// print error
	if (i >= count) {
		printk("'%s' is not found.\n", cmd_buf);
	}
}

static void param_init(param_list_t *param_list, const char* param) {
	param_list->buf = param;
	param_list->len = strlen(param);
	param_list->cur_pos = 0;
}

static int next_param(param_list_t* param_list, char* param) {
	int i, len;

	// if has no more parameter, just return
	if (param_list->len <= param_list->cur_pos) {
		return 0;
	}

	// find index of ' '
	for (i = param_list->cur_pos; i < param_list->len; i++) {
		if (param_list->buf[i] == ' ') {
			break;
		}
	}

	// copy parameter
	memcpy(param, param_list->buf + param_list->cur_pos, i);
	len = i - param_list->cur_pos;
	param[len] = '\0';

	// update position in parameter buffer
	param_list->cur_pos += len + 1;
	return len;
}

/* **********************************************************
 *  Built-in Command List
 * **********************************************************/

void print_task(tcb_t *task) {
	if (!task) {
		printk("No Task\n");
	} else {
		printk("%d %d %s %d", task->id, task->parent_process->id, task->name,
				task->core);
		printk(" %x %x", task->pml4, task->context.rsp);
		printk(" %d %x\n", task->fpu_used, task->flags);
	}
}

extern scheduler_t scheduler;

/*static*/void cmd_ps(const char* param_buf) {
	list_node_t *cur;
	int i;

	// print on-core task
	printk("\n#######################################\ncore-0: ");
	print_task(scheduler.cur_task[0]);
	printk("core-1: ");
	print_task(scheduler.cur_task[1]);
	printk("core-2: ");
	print_task(scheduler.cur_task[2]);
	printk("core-3: ");
	print_task(scheduler.cur_task[3]);

	// print ready task
	int count = ready_task_count();

	if (!count) {
		printk("no ready task\n");
	} else {
		printk("ready task count: %d\n", count);

		for (cur = scheduler.ready_list.head, i = 0; cur;
				cur = cur->next, i++) {
			printk("ready-%d: ", i);
			print_task(cur->value);
		}
	}
	printk("#######################################\n");
}
