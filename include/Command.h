#ifndef COMMAND_H
#define COMMAND_H

enum { 
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
	UPDATE_CMD,
	DELETE_CMD,
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
};

enum {
    oneOp = 1001,
    andOp,
    orOp,
};
enum {
    equalto = 2001,
    notEqualto,
    greater,
    less,
    greaterOrEqualto,
    lessOrEqualto,
    stringEqualto,
    stringNotEqualto,
};

enum {
	none = 3001,
    sum,
	count,
	avg,
};
enum {
	USERTABLE = 4001,
	LIKETABLE,
};

enum {
	UNJOINED = 5001,
	JOINED,
};
typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct SelectArgs {
    char **fields;
    size_t fields_len;
    int offset;
    int limit;
} SelectArgs_t;

typedef struct WhereArgs {
    char ** fields;
    size_t fields_len;
    size_t op_type1;//the > < = >= <= operator comparison and string comparison equal to or not equal to
    size_t op_type2;
    char* string_comp1;
    size_t num_comp1;
    char* string_comp2;
    size_t num_comp2;
    size_t op_num;//the number of operator, means and or,  for only one condition, 1 for and condition, 2 for or condition
}WhereArgs_t;

typedef struct AggeArgs {
	//set fields to multiple
	//set fields_len
    char ** fields;
	size_t fields_len;
	//set the type to multiple
    size_t agge_type[256];
}AggeArgs_t;

typedef struct UpdArgs {
    char * fields;
    char * str;
	size_t num;
}UpdArgs_t;


typedef struct JoinArgs {
	size_t JoinON;
    char * LeftField;
    char * RightField;
}JoinArgs_t;

typedef union {
    SelectArgs_t sel_args;
} CmdArg_t;

typedef union{
    WhereArgs_t whe_args;
} WheArg_t;

typedef union{
    AggeArgs_t agge_args;
} AggeArg_t;

typedef union{
    UpdArgs_t upd_args;
} UpdArg_t;

typedef union{
    JoinArgs_t join_args;
} JoinArg_t;

typedef struct Command {
    unsigned char type;
    char **args;
    size_t args_len;
    size_t args_cap;
	size_t user_or_like;
    CmdArg_t cmd_args;
    WheArg_t whe_args;
	AggeArg_t agge_args;
	UpdArg_t upd_args;
	JoinArg_t join_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument);
int add_where_field(Command_t *cmd, const char *argument);
int add_agge_field(Command_t *cmd, const char *argument);
int add_update_field(Command_t *cmd, const char *argument);
void cleanup_Command(Command_t *cmd);

#endif
