#include <stdlib.h>
#include <string.h>
#include "Command.h"
#include <stdio.h>
CMD_t cmd_list[] = {
    { ".exit", 5, BUILT_IN_CMD },
    { ".output", 7, BUILT_IN_CMD },
    { ".load", 5, BUILT_IN_CMD },
    { ".help", 5, BUILT_IN_CMD },
    { "insert", 6, QUERY_CMD },
    { "select", 6, QUERY_CMD },
	{ "update", 6, UPDATE_CMD },
	{ "delete", 6, DELETE_CMD },
    { "", 0, UNRECOG_CMD },
};

Command_t* new_Command() {
    Command_t *cmd = (Command_t*) malloc(sizeof(Command_t));
    memset(cmd, 0, sizeof(Command_t));
	
	cmd->user_or_like = USERTABLE;
    cmd->type = UNRECOG_CMD;
    cmd->args = NULL;
    cmd->args_len = 0;
    cmd->args_cap = 0;
    return cmd;
}

///
/// Add argument to Command_t
/// Reallocate buffer if it is full.
///
int add_Arg(Command_t *cmd, const char *arg) {
    char **new_buf;
    if (cmd->args == NULL) {
        new_buf = (char **) malloc(sizeof(char*) * 5);
        if (new_buf == NULL)
            goto error;

        cmd->args = new_buf;
        cmd->args_cap = 5;
        memset((void*)cmd->args, 0, sizeof(char*) * 5);
    } else if (cmd->args_cap == cmd->args_len) {
        new_buf = (char **) malloc(
                                sizeof(char*) * (cmd->args_cap + 5));
        if (new_buf == NULL)
            goto error;

        memset((void*)new_buf, 0,
            sizeof(char*) * (cmd->args_cap+5));
        memcpy((void*)new_buf, (void *)cmd->args,
            sizeof(char*) * (cmd->args_cap));
        free(cmd->args);
        cmd->args = new_buf;
        cmd->args_cap += 5;
    }
    cmd->args[cmd->args_len] = strdup(arg);
    cmd->args_len++;
    return 0;

error:
    return 1;
}

///
/// Add select fields
///
int add_select_field(Command_t *cmd, const char *argument) {
    size_t fields_len = cmd->cmd_args.sel_args.fields_len;
    char **buf = (char**)malloc(sizeof(char*) * (fields_len+1));
    if (buf == NULL) {
        return 0;
    }

    if (cmd->cmd_args.sel_args.fields) {
        memcpy(buf, cmd->cmd_args.sel_args.fields, sizeof(char*) * fields_len);
        free(cmd->cmd_args.sel_args.fields);
    }

    cmd->cmd_args.sel_args.fields = buf;
    cmd->cmd_args.sel_args.fields[fields_len] = strdup(argument);
    cmd->cmd_args.sel_args.fields_len++;
    return 1;
}

int add_where_field(Command_t *cmd, const char *argument) {
    size_t fields_len = cmd->whe_args.whe_args.fields_len;
    char **buf = (char**)malloc(sizeof(char*) * (fields_len+1));
    if (buf == NULL) {
        return 0;
    }

    if (cmd->whe_args.whe_args.fields) {
        memcpy(buf, cmd->whe_args.whe_args.fields, sizeof(char*) * fields_len);
        free(cmd->whe_args.whe_args.fields);
    }

    cmd->whe_args.whe_args.fields = buf;
    cmd->whe_args.whe_args.fields[fields_len] = strdup(argument);
    cmd->whe_args.whe_args.fields_len++;
    return 1;
}

int add_agge_field(Command_t *cmd, const char *argument) {
	
	size_t fields_len = cmd->agge_args.agge_args.fields_len;
    char **buf = (char**)malloc(sizeof(char*) * (fields_len+1));
    if (buf == NULL) {
        return 0;
    }

    if (cmd->agge_args.agge_args.fields) {
        memcpy(buf, cmd->agge_args.agge_args.fields, sizeof(char*) * fields_len);
        free(cmd->agge_args.agge_args.fields);
    }

    cmd->agge_args.agge_args.fields = buf;
    cmd->agge_args.agge_args.fields[fields_len] = strdup(argument);
    cmd->agge_args.agge_args.fields_len++;
	//change to add fields
    return 1;
}

int add_update_field(Command_t *cmd, const char *argument){
	cmd->upd_args.upd_args.fields = strdup(argument);
	return 1;
}
///
/// Free the allocated arguments, but without free the argument buffer
///
void cleanup_Command(Command_t *cmd) {
    size_t idx;
    for (idx = 0; idx < cmd->args_cap; idx++) {
        free(cmd->args[idx]);
        cmd->args[idx] = NULL;
    }
    if (cmd->type == SELECT_CMD) {
        for (idx = 0; idx < cmd->cmd_args.sel_args.fields_len; idx++) {
            free(cmd->cmd_args.sel_args.fields[idx]);
            cmd->cmd_args.sel_args.fields[idx] = NULL;
        }
        free(cmd->cmd_args.sel_args.fields);
        cmd->cmd_args.sel_args.fields = NULL;
        cmd->cmd_args.sel_args.fields_len = 0;
    }
    cmd->type = UNRECOG_CMD;
    cmd->args_len = 0;
	cmd->user_or_like  = 0;
	cmd->join_args.join_args.JoinON = UNJOINED;
}

