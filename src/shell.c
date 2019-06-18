#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InputBuffer.h"
#include "Util.h"
#include "Table.h"


int main(int argc, char **argv) {
    InputBuffer_t *input_buffer = new_InputBuffer();
    Command_t *cmd = new_Command();
    State_t *state = new_State();
    Table_t *table = NULL;
	TableLike_t *tablelike = NULL;
    int cmd_type;
    if (argc < 2) {
        table = new_Table(NULL);
		tablelike = new_Tablelike(NULL);
    } else {
        table = new_Table(argv[1]);
		tablelike = new_Tablelike(argv[2]);
    }
    if (table == NULL) {
        return 1;
    }
    for (;;) {
        print_prompt(state);
        read_input(input_buffer);
        cmd_type = parse_input(input_buffer->buffer, cmd);
        if (cmd_type == BUILT_IN_CMD) {
            handle_builtin_cmd(table,tablelike, cmd, state);
        } else if (cmd_type == QUERY_CMD) {
            handle_query_cmd(table,tablelike, cmd);
        } else if (cmd_type == UPDATE_CMD) {
            handle_update_cmd(table, cmd);	
        } else if (cmd_type == DELETE_CMD) {
            handle_delete_cmd(table, cmd);
        } else if (cmd_type == UNRECOG_CMD) {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
        }
        cleanup_Command(cmd);
        clean_InputBuffer(input_buffer);
    }
    return 0;
}
