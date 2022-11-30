#ifndef _UI_H_
#define _UI_H_
#include <sqlite3.h>
#include <stdio.h>

int prepare_db(sqlite3 **db, char *sql_stmt, sqlite3_stmt **ppStmt);



#endif /* defined(_UI_H_) */