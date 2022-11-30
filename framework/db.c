
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int prepare_db(sqlite3 *db, char *sql_stmt, sqlite3_stmt **ppStmt) {

    int rc = sqlite3_prepare_v2(db, sql_stmt, -1, ppStmt, NULL);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    return 0;
}

int initialize_db(sqlite3 *db) {
    char *sql;
    char *err_msg = 0;
    int rc;

    rc = sqlite3_open("chat.db", &db);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    sql = "CREATE TABLE IF NOT EXISTS Messages("  \
        "sender TEXT NOT NULL," \
        "receiver TEXT NOT NULL," \
        "time InDtTm DATETIME DEFAULT CURRENT_TIMESTAMP," \
        "message NVARCHAR(1024) NOT NULL);";

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    sql = "CREATE TABLE IF NOT EXISTS Users("  \
            "username TEXT NOT NULL," \
            "password TEXT NOT NULL," \
            "PRIMARY KEY(username));";
  
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    return 0;
}

int exec_query(sqlite3 *db, char *sql_stmt) {
    char *err_msg = 0;
    int rc;

    rc = sqlite3_exec(db, sql_stmt, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    return 0;
}