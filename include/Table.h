#ifndef TABLE_H
#define TABLE_H
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "User.h"

#define INIT_TABLE_SIZE 10000
#define EXT_LEN 500

#define MAX_int 1000000

typedef struct Table {
    bool   id[MAX_int];
    size_t capacity;
    size_t len;
    User_t *users;
    unsigned char *cache_map;
    FILE *fp;
    char *file_name;
} Table_t;

typedef struct Table_like {
    unsigned int id1[MAX_int] , id2[MAX_int];
    size_t capacity;
    size_t len;
    Like_t *likes;
    unsigned char *cache_map;
    FILE *fp;
    char *file_name;
} TableLike_t;

Table_t *new_Table(char *file_name);
TableLike_t *new_Tablelike(char *file_name);
int add_User(Table_t *table, User_t *user);
int add_Like(TableLike_t *tablelike, Like_t *like);

int minus_User(Table_t *table,size_t idx) ;
int rearrange_user(Table_t *table);
int archive_table(Table_t *table);
int archive_tablelike(TableLike_t *tablelike);

int load_table(Table_t *table, char *file_name);
int load_tablelike(TableLike_t *tablelike, char *file_name);

User_t* get_User(Table_t *table, size_t idx);

Like_t* get_Like(TableLike_t *tableLike, size_t idx);


#endif
