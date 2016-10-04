#ifndef BUILTINSHELL_H_
#define BUILTINSHELL_H_

#include "types.h"

#define SHELL_CMDBUF_MAX 300
#define SHELL_PROMPT     "B)"

typedef void (*cmd_handler_t)(const char* param);

#pragma pack( push, 1 )
typedef struct {
	char* cmd;
	cmd_handler_t handler;
} cmd_entry_t;

typedef struct {
	const char* buf;
	int len;
	int cur_pos;
} param_list_t;
#pragma pack( pop )

// shell core
void builtin_shell(void);
static void run_cmd(const char* cmd_buf);
static void param_init(param_list_t* param_list, const char* param);
static int next_param(param_list_t* param_list, char* param);

// command
/*static*/ void cmd_ps(const char* param_buf);

#endif /* BUILTINSHELL_H_ */
