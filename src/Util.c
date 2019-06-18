#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}
void print_like(Like_t *like, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %d", like->id1,like->id2);
        } else {
            if (idx > 0) printf(", ");
            if (!strncmp(sel_args->fields[idx], "id1", 3)) {
                printf("%d", like->id1);
            } else if (!strncmp(sel_args->fields[idx], "id2", 3)) {
                printf("%d", like->id2);
            }
        }
    }
    printf(")\n");
}
///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table,TableLike_t *tablelike, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    /*
    for(idx = 0;idx<cmd->whe_args.whe_args.fields_len;idx++){
	   printf("field:%s\n",cmd->whe_args.whe_args.fields[idx]);
    }
    printf("%ld\n",cmd->whe_args.whe_args.fields_len);
    printf("%ld\n",cmd->whe_args.whe_args.op_type1);
    printf("%ld\n",cmd->whe_args.whe_args.op_type2);
    printf("HELLO1\n");
    if(cmd->whe_args.whe_args.string_comp1)
    	printf("%s\n",cmd->whe_args.whe_args.string_comp1);
    if(cmd->whe_args.whe_args.string_comp2)
	printf("%s\n",cmd->whe_args.whe_args.string_comp2);
    printf("HELLO2\n");
    printf("%ld\n",cmd->whe_args.whe_args.num_comp1);
    printf("%ld\n",cmd->whe_args.whe_args.num_comp2);
    printf("%ld\n",cmd->whe_args.whe_args.op_num);
*/
	/*if(cmd->agge_args.agge_args.fields)
		printf("agge_field:%s\n",cmd->agge_args.agge_args.fields);
	printf("agge_type:%ld\n",cmd->agge_args.agge_args.agge_type);
*/

    if (offset == -1) {
        offset = 0;
    }


	/*printf("joinON:%ld\n",cmd->join_args.join_args.JoinON);
	printf("left:%s\n",cmd->join_args.join_args.LeftField);
	printf("right:%s\n",cmd->join_args.join_args.RightField);*/

	//the aggregate function
	if(cmd->agge_args.agge_args.agge_type[0]!=none){
		if(offset ==0)
			print_agge(table,tablelike,idxList,idxListLen,cmd);
		return;
	}

	//end of the aggregate function

	if(cmd->user_or_like == LIKETABLE){
		int offset_count = offset;
        for (idx = 0; idx < tablelike->len; idx++) {
            if (limit != -1 && (int)idx - (int)offset >= (int)limit) {
                break;
            }
			//if(check_condition(cmd,tablelike,idx)){
				offset_count--;
				if(offset_count<0)
					print_like(get_Like(tablelike, idx), &(cmd->cmd_args.sel_args));
			//}
			else{
				if(limit != -1 && offset == 0){
					limit++;
				}
				else if(offset != 0 && limit != -1){
					limit++;
				}
				//printf("offset:%d\n",offset);
			}
        }
    }
	else if(cmd->join_args.join_args.JoinON==JOINED){
		int offset_count = offset;
		//printf("HI!\n");
        for (idx = 0; idx < table->len; idx++) {
            if (limit != -1 && (int)idx - (int)offset >= (int)limit) {
                break;
            }
			if(check_condition(cmd,table,idx)){
				int idx_join = 0;
				for(idx_join = 0; idx_join < tablelike->len; idx_join++){
					Like_t *tempPtr = get_Like(tablelike,idx_join);
					if(check_equal(tempPtr,cmd,table,idx)){
						offset_count--;
						if(offset_count<0){
							print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
						}

					}
				}

			}
			else{
				if(limit != -1 && offset == 0){
					limit++;
				}
				else if(offset != 0 && limit != -1){
					limit++;
				}
				//printf("offset:%d\n",offset);
			}
        }
	}
	else {
		int offset_count = offset;
        for (idx = 0; idx < table->len; idx++) {
            if (limit != -1 && (int)idx - (int)offset >= (int)limit) {
                break;
            }
			if(check_condition(cmd,table,idx)){
				offset_count--;
				if(offset_count<0)
					print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
			}
			else{
				if(limit != -1 && offset == 0){
					limit++;
				}
				else if(offset != 0 && limit != -1){
					limit++;
				}
				//printf("offset:%d\n",offset);
			}
        }
    }
}

void print_agge(Table_t *table, TableLike_t *tablelike,int *idxList, size_t idxListLen, Command_t *cmd){

	//first check agge_type then calculate fields in each type, at last output
	size_t agge_idx = 0;
	/*for(agge_idx = 0;agge_idx < cmd->agge_args.agge_args.fields_len;agge_idx++){
		printf("agge_idx:%ld,agge_type:%ld\n",agge_idx,cmd->agge_args.agge_args.agge_type[agge_idx]);
		printf("agge_idx:%ld,agge_fields:%s\n",agge_idx,cmd->agge_args.agge_args.fields[agge_idx]);
	}*/

	/*printf("%s\n",cmd->join_args.join_args.LeftField);
	printf("%s\n",cmd->join_args.join_args.RightField);
	printf("%ld\n",cmd->join_args.join_args.JoinON);*/
	printf("(");

	if(cmd->user_or_like == LIKETABLE){
		for(agge_idx = 0 ;agge_idx < cmd->agge_args.agge_args.fields_len;agge_idx++){
		//
		if(cmd->agge_args.agge_args.agge_type[agge_idx] == sum){
			int result_sumId1 = 0;
			int result_sumId2 = 0;
			size_t idx = 0;
			Like_t *temp_like = NULL;
			for (idx = 0; idx < tablelike->len; idx++) {
				temp_like = get_Like(tablelike, idx);
				if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id1",3)){
					result_sumId1 += temp_like->id1;
				} else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id2",3))
					result_sumId2 += temp_like->id2;
			}
			if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id1",3))
				printf("%d",result_sumId1);
			else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id3",3))
				printf("%d",result_sumId2);
		} else if(cmd->agge_args.agge_args.agge_type[agge_idx]== avg){
			double result_avgId1 = 0;
			double result_avgId2 = 0;
			size_t idx = 0;
			int base = 0;
			Like_t *temp_like = NULL;
			for (idx = 0; idx < tablelike->len; idx++) {
				temp_like = get_Like(tablelike, idx);
				base++;
				if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id1",3)){
					result_avgId1 += temp_like->id1;
				} else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id2",3))
					result_avgId2 += temp_like->id2;
			}
			result_avgId1/=base;
			result_avgId2/=base;
			if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id1",2))
				printf("%.3f",result_avgId1);
			else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id2",3))
				printf("%.3f",result_avgId2);
		} else if(cmd->agge_args.agge_args.agge_type[agge_idx] == count){
			int count = 0;
			size_t idx = 0;
			for (idx = 0; idx < tablelike->len; idx++){
				count++;
			}
			printf("%d",count);
		}
		//
		if(agge_idx != cmd->agge_args.agge_args.fields_len-1)
			printf(", ");
	}


		printf(")\n");
		return;
	}


	for(agge_idx = 0 ;agge_idx < cmd->agge_args.agge_args.fields_len;agge_idx++){
		//
		if(cmd->agge_args.agge_args.agge_type[agge_idx] == sum){
			int result_sumId = 0;
			int result_sumAge = 0;
			size_t idx = 0;
			User_t *temp_user = NULL;



			for (idx = 0; idx < table->len; idx++) {
				temp_user = get_User(table, idx);
				if(check_condition(cmd,table,idx)){

					if(cmd->join_args.join_args.JoinON == JOINED){
						int idx_join = 0;
						for(idx_join = 0; idx_join < tablelike->len; idx_join++){
							Like_t *tempPtr = get_Like(tablelike,idx_join);
							if(check_equal(tempPtr,cmd,table,idx)){
								result_sumId+=temp_user->id;
								result_sumAge+=temp_user->age;
							}
						}
					}
					else{
						if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id",2)){
							result_sumId += temp_user->id;
						}
						else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"age",3))
							result_sumAge += temp_user->age;
					}

				}
			}
			if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id",2))
				printf("%d",result_sumId);
			else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"age",3))
				printf("%d",result_sumAge);
		} else if(cmd->agge_args.agge_args.agge_type[agge_idx]== avg){
			double result_avgId = 0;
			double result_avgAge = 0;
			size_t idx = 0;
			int base = 0;
			User_t *temp_user = NULL;
			for (idx = 0; idx < table->len; idx++) {
				temp_user = get_User(table, idx);
				if(check_condition(cmd,table,idx)){
					if(cmd->join_args.join_args.JoinON == JOINED){
						int idx_join = 0;
						for(idx_join = 0; idx_join < tablelike->len; idx_join++){
							Like_t *tempPtr = get_Like(tablelike,idx_join);
							if(check_equal(tempPtr,cmd,table,idx)){
								base++;
								if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id",2))
									result_avgId += temp_user->id;
								else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"age",3))
									result_avgAge += temp_user->age;
							}
						}

					}
					else{
						base++;
						if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id",2)){
							result_avgId += temp_user->id;
						} else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"age",3))
							result_avgAge += temp_user->age;
					}

				}
			}
			result_avgId/=base;
			result_avgAge/=base;
			if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"id",2))
				printf("%.3f",result_avgId);
			else if(!strncmp(cmd->agge_args.agge_args.fields[agge_idx],"age",3))
				printf("%.3f",result_avgAge);
		} else if(cmd->agge_args.agge_args.agge_type[agge_idx] == count){
			int count = 0;
			size_t idx = 0;
            if(cmd->join_args.join_args.JoinON == JOINED){
                if(!strncmp(cmd->join_args.join_args.RightField,"id1",3)){
                    for (idx = 0; idx < table->len; idx++) {
        				if(check_condition(cmd,table,idx))
                            count += tablelike->id1[get_User(table,idx)->id];
                    }
                }
                else if(!strncmp(cmd->join_args.join_args.RightField,"id2",3)){
                    for (idx = 0; idx < table->len; idx++) {
                        if(check_condition(cmd,table,idx))
                            count += tablelike->id2[get_User(table,idx)->id];
                    }
                }
            }
            else{
                for (idx = 0; idx < table->len; idx++) {
    				if(check_condition(cmd,table,idx))
                        count++;
                }
            }
            printf("%d",count);
			/*for (idx = 0; idx < table->len; idx++) {
				if(check_condition(cmd,table,idx)){
					if(cmd->join_args.join_args.JoinON == JOINED){
						int idx_join = 0;
						for(idx_join = 0; idx_join < tablelike->len; idx_join++){
							Like_t *tempPtr = get_Like(tablelike,idx_join);
							if(check_equal(tempPtr,cmd,table,idx)){
								count++;
							}
						}
					}
					else
						count++;
				}
			}*/

		}
		//
		if(agge_idx != cmd->agge_args.agge_args.fields_len-1)
			printf(", ");
	}
	printf(")\n");



}

int check_condition(Command_t *cmd, Table_t *table,size_t idx){
    User_t *temp_user = NULL;
	size_t result1  = 0;
	size_t result2  = 0;
    if(cmd->whe_args.whe_args.fields_len == 0){
		return 1;
	} else if(cmd->whe_args.whe_args.fields_len > 0 ){
		temp_user = get_User(table,idx);
		if(cmd->whe_args.whe_args.op_type1 == stringEqualto || cmd->whe_args.whe_args.op_type1 == stringNotEqualto){
			if(!strncmp(cmd->whe_args.whe_args.fields[0],"name",4)){
				if(cmd->whe_args.whe_args.op_type1 == stringEqualto){
					if( !strcmp(cmd->whe_args.whe_args.string_comp1,temp_user->name) ){
						result1 = 1;}
					} else if(cmd->whe_args.whe_args.op_type1 == stringNotEqualto){
						if( strcmp(cmd->whe_args.whe_args.string_comp1,temp_user->name) ){
							result1 = 1;}
					}
				} else if (!strncmp(cmd->whe_args.whe_args.fields[0],"email",5)){
					if(cmd->whe_args.whe_args.op_type1 == stringEqualto){
						if( !strcmp(cmd->whe_args.whe_args.string_comp1,temp_user->email) ){
							result1 = 1;}
						} else if(cmd->whe_args.whe_args.op_type1 == stringNotEqualto){
							if( strcmp(cmd->whe_args.whe_args.string_comp1,temp_user->email) ){
								result1 = 1;}
					}
			}
		}
		//the num comparison
		else if(cmd->whe_args.whe_args.op_type1 >= equalto && cmd->whe_args.whe_args.op_type1 <= lessOrEqualto){
			//id check
			if(!strncmp(cmd->whe_args.whe_args.fields[0],"id",2)){
				if(cmd->whe_args.whe_args.op_type1 == equalto){
					if( temp_user->id == cmd->whe_args.whe_args.num_comp1) {
						result1 = 1;}
					} else if(cmd->whe_args.whe_args.op_type1 == notEqualto){
						if( temp_user->id != cmd->whe_args.whe_args.num_comp1) {
							result1 = 1;}
					} else if(cmd->whe_args.whe_args.op_type1 == greater){
						if( temp_user->id > cmd->whe_args.whe_args.num_comp1) {
							result1 = 1;}
					} else if(cmd->whe_args.whe_args.op_type1 == less){
						if( temp_user->id < cmd->whe_args.whe_args.num_comp1) {
							result1 = 1;}
					} else if(cmd->whe_args.whe_args.op_type1 == greaterOrEqualto){
						if( temp_user->id >= cmd->whe_args.whe_args.num_comp1) {
							result1 = 1;}
					} else if(cmd->whe_args.whe_args.op_type1 == lessOrEqualto){
						if( temp_user->id <= cmd->whe_args.whe_args.num_comp1) {
							result1 = 1;}
					}
					//age
				} else if (!strncmp(cmd->whe_args.whe_args.fields[0],"age",3)){
					if(cmd->whe_args.whe_args.op_type1 == equalto){
						if( temp_user->age == cmd->whe_args.whe_args.num_comp1) {
							result1 = 1;}
						} else if(cmd->whe_args.whe_args.op_type1 == notEqualto){
							if( temp_user->age != cmd->whe_args.whe_args.num_comp1) {
								result1 = 1;}
						} else if(cmd->whe_args.whe_args.op_type1 == greater){
							if( temp_user->age > cmd->whe_args.whe_args.num_comp1) {
								result1 = 1;}
						} else if(cmd->whe_args.whe_args.op_type1 == less){
							if( temp_user->age < cmd->whe_args.whe_args.num_comp1) {
								result1 = 1;}
						} else if(cmd->whe_args.whe_args.op_type1 == greaterOrEqualto){
							if( temp_user->age >= cmd->whe_args.whe_args.num_comp1) {
								result1 = 1;}
						} else if(cmd->whe_args.whe_args.op_type1 == lessOrEqualto){
							if( temp_user->age <= cmd->whe_args.whe_args.num_comp1){
								result1 = 1;}
					}
			}
		}
		//return result1;
		//the second where judge
	}
	if(cmd->whe_args.whe_args.fields_len == 1)
		return result1;
	if(cmd->whe_args.whe_args.fields_len == 2){
		temp_user = get_User(table,idx);
		if(cmd->whe_args.whe_args.op_type2 == stringEqualto || cmd->whe_args.whe_args.op_type2 == stringNotEqualto){
			if(!strncmp(cmd->whe_args.whe_args.fields[1],"name",4)){
				if(cmd->whe_args.whe_args.op_type2 == stringEqualto){
					if( !strcmp(cmd->whe_args.whe_args.string_comp2,temp_user->name) ){
						result2 = 1;}
					} else if(cmd->whe_args.whe_args.op_type2 == stringNotEqualto){
						if( strcmp(cmd->whe_args.whe_args.string_comp2,temp_user->name) ){
							result2 = 1;}
					}
				} else if (!strncmp(cmd->whe_args.whe_args.fields[1],"email",5)){
					if(cmd->whe_args.whe_args.op_type2 == stringEqualto){
						if( !strcmp(cmd->whe_args.whe_args.string_comp2,temp_user->email) ){
							result2 = 1;}
						} else if(cmd->whe_args.whe_args.op_type2 == stringNotEqualto){
							if( strcmp(cmd->whe_args.whe_args.string_comp2,temp_user->email) ){
								result2 = 1;}
					}
			}
		}
		//the num comparison
		else if(cmd->whe_args.whe_args.op_type2 >= equalto && cmd->whe_args.whe_args.op_type2 <= lessOrEqualto){
			//id check
			if(!strncmp(cmd->whe_args.whe_args.fields[1],"id",2)){
				if(cmd->whe_args.whe_args.op_type2 == equalto){
					if( temp_user->id == cmd->whe_args.whe_args.num_comp2) {
						result2 = 1;}
					} else if(cmd->whe_args.whe_args.op_type2 == notEqualto){
						if( temp_user->id != cmd->whe_args.whe_args.num_comp2) {
							result2 = 1;}
					} else if(cmd->whe_args.whe_args.op_type2 == greater){
						if( temp_user->id > cmd->whe_args.whe_args.num_comp2) {
							result2 = 1;}
					} else if(cmd->whe_args.whe_args.op_type2 == less){
						if( temp_user->id < cmd->whe_args.whe_args.num_comp2) {
							result2 = 1;}
					} else if(cmd->whe_args.whe_args.op_type2 == greaterOrEqualto){
						if( temp_user->id >= cmd->whe_args.whe_args.num_comp2) {
							result2 = 1;}
					} else if(cmd->whe_args.whe_args.op_type2 == lessOrEqualto){
						if( temp_user->id <= cmd->whe_args.whe_args.num_comp2) {
							result2 = 1;}
					}
					//age
				} else if (!strncmp(cmd->whe_args.whe_args.fields[1],"age",3)){
					if(cmd->whe_args.whe_args.op_type2 == equalto){
						if( temp_user->age == cmd->whe_args.whe_args.num_comp2) {
							result2 = 1;}
						} else if(cmd->whe_args.whe_args.op_type2 == notEqualto){
							if( temp_user->age != cmd->whe_args.whe_args.num_comp2) {
								result2 = 1;}
						} else if(cmd->whe_args.whe_args.op_type2 == greater){
							if( temp_user->age > cmd->whe_args.whe_args.num_comp2) {
								result2 = 1;}
						} else if(cmd->whe_args.whe_args.op_type2 == less){
							if( temp_user->age < cmd->whe_args.whe_args.num_comp2) {
								result2 = 1;}
						} else if(cmd->whe_args.whe_args.op_type2 == greaterOrEqualto){
							if( temp_user->age >= cmd->whe_args.whe_args.num_comp2) {
								result2 = 1;}
						} else if(cmd->whe_args.whe_args.op_type2 == lessOrEqualto){
							if( temp_user->age <= cmd->whe_args.whe_args.num_comp2){
								result2 = 1;}
					}
			}
		}
		//judge the op_num and return;
		if(cmd->whe_args.whe_args.op_num == andOp)
			return (result1 && result2);
		else if(cmd->whe_args.whe_args.op_num == orOp)
			return (result1 || result2);
	}
	return 1;
}

int check_equal(Like_t *like,Command_t *cmd,Table_t *table,size_t idx){
	User_t * userptr= get_User(table,idx);
	if(!strncmp(cmd->join_args.join_args.LeftField,"id",2)){
		if(!strncmp(cmd->join_args.join_args.RightField,"id1",3)){
			if(userptr->id == like->id1){
				return 1;
			}
		}
		else if(!strncmp(cmd->join_args.join_args.RightField,"id2",3)){
			if(userptr->id == like->id2){
				return 1;
			}
		}
	}
	else if(!strncmp(cmd->join_args.join_args.LeftField,"age",3)){
		if(!strncmp(cmd->join_args.join_args.RightField,"id1",3)){
			if(userptr->age == like->id1){
				return 1;
			}
		}
		else if(!strncmp(cmd->join_args.join_args.RightField,"id2",3)){
			if(userptr->age == like->id2){
				return 1;
			}
		}
	}
	return 0;
}


///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, TableLike_t *tablelike,Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
		archive_tablelike(tablelike);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table,TableLike_t *tablelike, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
		if(!strncmp(cmd->args[2],"user",4)){
			handle_insert_cmd(table, cmd);
			return INSERT_CMD;
		}
		else if(!strncmp(cmd->args[2],"like",4)){
			handle_insertlike_cmd(tablelike, cmd);
			return INSERT_CMD;
		}
        else{
			return UNRECOG_CMD;
		}
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table,tablelike, cmd);
        return SELECT_CMD;
    } else {
        return UNRECOG_CMD;
    }
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}
int handle_insertlike_cmd(TableLike_t *tablelike, Command_t *cmd) {
    int ret = 0;
    Like_t *like = command_to_Like(cmd);
    if (like) {
        ret = add_Like(tablelike, like);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}
///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table,TableLike_t *tablelike, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);

    print_users(table,tablelike, NULL, 0, cmd);
    return table->len;
}
int handle_update_cmd(Table_t *table, Command_t *cmd) {
	cmd->type = UPDATE_CMD;
	update_state_handler(cmd, 1);
    size_t idx =0;
	/*
	if(cmd->upd_args.upd_args.fields)
		printf("%s\n",cmd->upd_args.upd_args.fields);
	if(cmd->upd_args.upd_args.str)
		printf("%s\n",cmd->upd_args.upd_args.str);
	printf("%ld\n",cmd->upd_args.upd_args.num);
	*/
	size_t pri_idx = 0;
	unsigned int pri_id = 0;
	int pri_enterwhere = 1;
	for (idx = 0;idx<table->len;idx++){
		if(check_condition(cmd,table,idx)){
			User_t *temp_user = get_User(table,idx);
			if( !strncmp(cmd->upd_args.upd_args.fields,"id",2)){
				//make sure no replicate primary_key
				if(pri_enterwhere>0){

					size_t check_idx = 0;
					int check_signal = 1;
					for(check_idx = 0;check_idx<table->len;check_idx++){
						User_t *check_user = get_User(table,check_idx);
						if(check_user->id == cmd->upd_args.upd_args.num && check_idx!=idx)
							check_signal = 0;
					}
					if(check_signal){
						pri_idx = idx;
						pri_id = cmd->upd_args.upd_args.num;
						temp_user->id = cmd->upd_args.upd_args.num;
						pri_enterwhere--;
					}

				}
				else{
					User_t *pri_user = get_User(table,pri_idx);
					pri_user->id = pri_id;
				}
			} else if(!strncmp(cmd->upd_args.upd_args.fields,"age",3)){
				temp_user->age = cmd->upd_args.upd_args.num;
			} else if(!strncmp(cmd->upd_args.upd_args.fields,"name",4)){
				strncpy(temp_user->name, cmd->upd_args.upd_args.str, MAX_USER_NAME);
			} else if(!strncmp(cmd->upd_args.upd_args.fields,"email",5)){
				strncpy(temp_user->email, cmd->upd_args.upd_args.str, MAX_USER_NAME);
			}
		}
	}
    return table->len;
}

int handle_delete_cmd(Table_t *table, Command_t *cmd) {
	cmd->type = DELETE_CMD;
	delete_state_handler(cmd, 1);
    size_t idx =0;
	/*
    for(idx = 0;idx<cmd->whe_args.whe_args.fields_len;idx++){
	   printf("field:%s\n",cmd->whe_args.whe_args.fields[idx]);
    }
    printf("%ld\n",cmd->whe_args.whe_args.fields_len);
    printf("%ld\n",cmd->whe_args.whe_args.op_type1);
    printf("%ld\n",cmd->whe_args.whe_args.op_type2);
    printf("HELLO1\n");
    if(cmd->whe_args.whe_args.string_comp1)
    	printf("%s\n",cmd->whe_args.whe_args.string_comp1);
    if(cmd->whe_args.whe_args.string_comp2)
	printf("%s\n",cmd->whe_args.whe_args.string_comp2);
    printf("HELLO2\n");
    printf("%ld\n",cmd->whe_args.whe_args.num_comp1);
    printf("%ld\n",cmd->whe_args.whe_args.num_comp2);
    printf("%ld\n",cmd->whe_args.whe_args.op_num);
*/
	for(idx = 0;idx<table->len;idx++){
		if(check_condition(cmd,table,idx))
			minus_User(table, idx) ;
	}
	rearrange_user(table);

    return table->len;
}
///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}
