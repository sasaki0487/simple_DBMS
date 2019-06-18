#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "Table.h"

///
/// Allocate a Table_t struct, then initialize some attributes, and
/// load table if the `file_name` is given
///
Table_t *new_Table(char *file_name) {
    Table_t *table = (Table_t*)malloc(sizeof(Table_t));
    memset((void*)table, 0, sizeof(Table_t));
    for(int i = 0 ; i < MAX_int ; i++){
        table->id[i] = false;
    }
    table->capacity = INIT_TABLE_SIZE;
    table->len = 0;
    table->users = (User_t*)malloc(
                            sizeof(User_t) * INIT_TABLE_SIZE);
    table->cache_map = (unsigned char*)malloc(sizeof(char)*INIT_TABLE_SIZE);
    memset(table->cache_map, 0, sizeof(char)*INIT_TABLE_SIZE);
    table->fp = NULL;
    table->file_name = NULL;
    load_table(table, file_name);
    return table;
}

TableLike_t *new_Tablelike(char *file_name) {
    TableLike_t *tablelike = (TableLike_t*)malloc(sizeof(TableLike_t));
    memset((void*)tablelike, 0, sizeof(TableLike_t));
    for(int i = 0 ; i < MAX_int ; i++){
        tablelike->id1[i] = 0;
        tablelike->id2[i] = 0;
    }
    tablelike->capacity = INIT_TABLE_SIZE;
    tablelike->len = 0;
    tablelike->likes = (Like_t*)malloc(
                            sizeof(Like_t) * INIT_TABLE_SIZE);
    tablelike->cache_map = (unsigned char*)malloc(sizeof(char)*INIT_TABLE_SIZE);
    memset(tablelike->cache_map, 0, sizeof(char)*INIT_TABLE_SIZE);
    tablelike->fp = NULL;
    tablelike->file_name = NULL;
    load_tablelike(tablelike, file_name);
    return tablelike;
}

///
/// Add the `User_t` data to the given table
/// If the table is full, it will allocate new space to store more
/// user data
/// return 1 when the data successfully add to table
///
int add_User(Table_t *table, User_t *user) {
    size_t idx;
    User_t *usr_ptr;
    if (!table || !user) {
        return 0;
    }
    // Check id doesn't exist in the table
    if ( table->id[user->id] ){
        return 0;
    }
    /*for (idx = 0; idx < table->len; idx++) {
        usr_ptr = get_User(table, idx);
        if (usr_ptr->id == user->id) {
            return 0;
        }
    }*/
    if (table->len == table->capacity) {
        User_t *new_user_buf = (User_t*)malloc(sizeof(User_t)*(table->len+EXT_LEN));
        unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(table->len+EXT_LEN));

        memcpy(new_user_buf, table->users, sizeof(User_t)*table->len);

        memset(new_cache_buf, 0, sizeof(unsigned char)*(table->len+EXT_LEN));
        memcpy(new_cache_buf, table->cache_map, sizeof(unsigned char)*table->len);


        free(table->users);
        free(table->cache_map);
        table->users = new_user_buf;
        table->cache_map = new_cache_buf;
        table->capacity += EXT_LEN;
    }
    table->id[user->id] = true;
    idx = table->len;
    memcpy((table->users)+idx, user, sizeof(User_t));
    table->cache_map[idx] = 1;
    table->len++;
    return 1;
}
int add_Like(TableLike_t *tablelike, Like_t *like) {
    size_t idx;
    //Like_t *like_ptr;
    if (!tablelike || !like) {
        return 0;
    }
    // Check id doesn't exist in the table
    /*
	for (idx = 0; idx < tablelike->len; idx++) {
        like_ptr = get_Like(tablelike, idx);
        if (like_ptr->id1 == like->id1) {
            return 0;
        }
    }*/
    if (tablelike->len == tablelike->capacity) {
        Like_t *new_like_buf = (Like_t*)malloc(sizeof(Like_t)*(tablelike->len+EXT_LEN));
        unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(tablelike->len+EXT_LEN));

        memcpy(new_like_buf, tablelike->likes, sizeof(Like_t)*tablelike->len);

        memset(new_cache_buf, 0, sizeof(unsigned char)*(tablelike->len+EXT_LEN));
        memcpy(new_cache_buf, tablelike->cache_map, sizeof(unsigned char)*tablelike->len);


        free(tablelike->likes);
        free(tablelike->cache_map);
        tablelike->likes = new_like_buf;
        tablelike->cache_map = new_cache_buf;
        tablelike->capacity += EXT_LEN;
    }
    tablelike->id1[like->id1]++;
    tablelike->id2[like->id2]++;
    idx = tablelike->len;
    memcpy((tablelike->likes)+idx, like, sizeof(Like_t));
    tablelike->cache_map[idx] = 1;
    tablelike->len++;
    return 1;
}

int minus_User(Table_t *table, size_t idx) {
    if (!table) {
        return 0;
    }
	User_t *usr_ptr = get_User(table, idx);
    table->id[usr_ptr->id] = false;
	usr_ptr->id = MAX_int;
    return 1;
}
int rearrange_user(Table_t *table) {
    if (!table) {
        return 0;
    }
	size_t idx = 0;
	size_t new_idx = 0;
	size_t minus_count = 0;
	User_t *usr_ptr = NULL;

	User_t *new_user_buf = (User_t*)malloc(sizeof(User_t)*(table->capacity));
    unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(table->capacity));
    memset(new_cache_buf, 0, sizeof(unsigned char)*(table->len));
    memcpy(new_cache_buf, table->cache_map, sizeof(unsigned char)*table->len);

	for(idx = 0;idx<table->len;idx++){
		usr_ptr = get_User(table, idx);
		if(usr_ptr->id != MAX_int){
			memcpy(new_user_buf + new_idx,usr_ptr,sizeof(User_t));
			memcpy(new_cache_buf + new_idx, table->cache_map + sizeof(unsigned char) * idx, sizeof(unsigned char));
			new_idx++;
			/*
			printf("usr_ptr->id:%d ",usr_ptr->id);
			User_t * test_ptr = NULL;
			test_ptr = new_user_buf -1 + new_idx;
			printf("new_user_buf_id:%d\n",test_ptr->id);
			*/
		}
		else minus_count++;
	}
	table->users = new_user_buf;
    table->cache_map = new_cache_buf;
	table->len -= minus_count;
    return 1;
}

///
/// Return value is the archived table len
///
int archive_table(Table_t *table) {
    size_t archived_len;
    struct stat st;

    if (table->fp == NULL) {
        return 0;
    }
    if (stat(table->file_name, &st) == 0) {
        archived_len = st.st_size / sizeof(User_t);
    } else {
        archived_len = 0;
    }
    fwrite((void*)(table->users+archived_len), \
            sizeof(User_t), table->len-archived_len, \
            table->fp);

    fclose(table->fp);
    free(table->file_name);
    table->fp = NULL;
    table->file_name = NULL;
    return table->len;
}
int archive_tablelike(TableLike_t *tablelike) {
    size_t archived_len;
    struct stat st;
	/*printf("tablelike len:%ld\n",tablelike->len);
	printf("%s\n",tablelike->file_name);*/
    if (tablelike->fp == NULL) {
        return 0;
    }
    if (stat(tablelike->file_name, &st) == 0) {
        archived_len = st.st_size / sizeof(Like_t);
    } else {
        archived_len = 0;
    }
    fwrite((void*)(tablelike->likes+archived_len), \
            sizeof(Like_t), tablelike->len-archived_len, \
            tablelike->fp);

    fclose(tablelike->fp);
    free(tablelike->file_name);
    tablelike->fp = NULL;
    tablelike->file_name = NULL;
    return tablelike->len;
}
///
/// Loading the db file will overwrite the existed records in table,
/// only if the ``file_name`` is NULL
/// Return: the number of records in the db file
///
int load_table(Table_t *table, char *file_name) {
    size_t archived_len;
    struct stat st;
    if (table->fp != NULL) {
        fclose(table->fp);
        free(table->file_name);
        table->fp = NULL;
        table->file_name = NULL;
    }
    if (file_name != NULL) {
        table->len = 0;
        memset(table->cache_map, 0, sizeof(char)*INIT_TABLE_SIZE);
        if (stat(file_name, &st) != 0) {
            //Create new file
            table->fp = fopen(file_name, "wb");
        } else {
            archived_len = st.st_size / sizeof(User_t);
            if (archived_len > table->capacity) {
                User_t *new_user_buf = (User_t*)malloc(sizeof(User_t)*(archived_len+EXT_LEN));
                unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(archived_len+EXT_LEN));

                memset(new_cache_buf, 0, sizeof(unsigned char)*(archived_len+EXT_LEN));

                free(table->users);
                free(table->cache_map);
                table->users = new_user_buf;
                table->cache_map = new_cache_buf;
                table->capacity = archived_len+EXT_LEN;
            }
            table->fp = fopen(file_name, "a+b");
            table->len = archived_len;
        }
        table->file_name = strdup(file_name);
    }
    return table->len;
}

int load_tablelike(TableLike_t *tablelike, char *file_name) {
    size_t archived_len;
    struct stat st;
    if (tablelike->fp != NULL) {
        fclose(tablelike->fp);
        free(tablelike->file_name);
        tablelike->fp = NULL;
        tablelike->file_name = NULL;
    }
    if (file_name != NULL) {
        tablelike->len = 0;
        memset(tablelike->cache_map, 0, sizeof(char)*INIT_TABLE_SIZE);
        if (stat(file_name, &st) != 0) {
            //Create new file
            tablelike->fp = fopen(file_name, "wb");
        } else {
            archived_len = st.st_size / sizeof(User_t);
            if (archived_len > tablelike->capacity) {
                Like_t *new_like_buf = (Like_t*)malloc(sizeof(Like_t)*(archived_len+EXT_LEN));
                unsigned char *new_cache_buf = (unsigned char *)malloc(sizeof(unsigned char)*(archived_len+EXT_LEN));

                memset(new_cache_buf, 0, sizeof(unsigned char)*(archived_len+EXT_LEN));

                free(tablelike->likes);
                free(tablelike->cache_map);
                tablelike->likes = new_like_buf;
                tablelike->cache_map = new_cache_buf;
                tablelike->capacity = archived_len+EXT_LEN;
            }
            tablelike->fp = fopen(file_name, "a+b");
            tablelike->len = archived_len;
        }
        tablelike->file_name = strdup(file_name);
    }
    return tablelike->len;
}

///
/// Return the user in table by the given index
///
User_t* get_User(Table_t *table, size_t idx) {
    size_t archived_len;
    struct stat st;
    if (!table->cache_map[idx]) {
        if (idx > INIT_TABLE_SIZE) {
            goto error;
        }
        if (stat(table->file_name, &st) != 0) {
            goto error;
        }
        archived_len = st.st_size / sizeof(User_t);
        if (idx >= archived_len) {
            //neither in file, nor in memory
            goto error;
        }

        fseek(table->fp, idx*sizeof(User_t), SEEK_SET);
        fread(table->users+idx, sizeof(User_t), 1, table->fp);
        table->cache_map[idx] = 1;
    }
    return table->users+idx;

error:
    return NULL;
}

Like_t* get_Like(TableLike_t *tablelike, size_t idx) {
    size_t archived_len;
    struct stat st;
    if (!tablelike->cache_map[idx]) {
        if (idx > INIT_TABLE_SIZE) {
            goto error;
        }
        if (stat(tablelike->file_name, &st) != 0) {
            goto error;
        }
        archived_len = st.st_size / sizeof(Like_t);
        if (idx >= archived_len) {
            //neither in file, nor in memory
            goto error;
        }

        fseek(tablelike->fp, idx*sizeof(Like_t), SEEK_SET);
        fread(tablelike->likes+idx, sizeof(Like_t), 1, tablelike->fp);
        tablelike->cache_map[idx] = 1;
    }
    return tablelike->likes+idx;

error:
    return NULL;
}
