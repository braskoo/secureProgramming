#ifndef _UI_H_
#define _UI_H_
#include <sqlite3.h>
#include <stdio.h>
#include "types.h"

int prepare_db(sqlite3 *db, char *sql_stmt, sqlite3_stmt **ppStmt);
int initialize_db(sqlite3 **db);
int exec_query(sqlite3 *db, char *sql_stmt);
void load_msgs(struct api_state *api, sqlite3_stmt *stmt);
void load_users(struct api_state *api, sqlite3_stmt *stmt);
char* check_users(struct api_state *api, sqlite3_stmt *stmt);

#endif /* defined(_UI_H_) */