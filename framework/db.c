
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "api.h"

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
    char *sql = malloc( (161 * sizeof(char)) + 1);
    if (sql == NULL) {
        printf("Memory not allocated.\n");
        return -1;
    }

    char *err_msg = 0;
    int rc;

    rc = sqlite3_open("chat.db", &db);
    if (rc != SQLITE_OK) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    strcpy(sql, "CREATE TABLE IF NOT EXISTS Messages("  \
        "sender TEXT NOT NULL," \
        "receiver TEXT NOT NULL," \
        "time InDtTm DATETIME DEFAULT CURRENT_TIMESTAMP," \
        "message NVARCHAR(1024) NOT NULL);");
        
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    sql = realloc(sql, 174*sizeof(char));

    strcpy(sql, "CREATE TABLE IF NOT EXISTS Users("  \
            "username TEXT NOT NULL," \
            "password TEXT NOT NULL," \
            "status INTEGER NOT NULL," \
            "time InDtTm DATETIME DEFAULT CURRENT_TIMESTAMP," \
            "PRIMARY KEY(username));");
  
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        printf("error: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    free(sql);
    sqlite3_close(db);
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

void load_msgs(struct api_state *api, sqlite3_stmt *stmt){
    int length = 0; 
    const unsigned char *time, *message, *sender, *receiver;
    char* msg = malloc(0);
    union CODE code = {C_PUBMSG};
    struct api_msg* notifs;

    while(sqlite3_step(stmt) == SQLITE_ROW){
        length = sqlite3_column_bytes(stmt, 0) + sqlite3_column_bytes(stmt, 1) + sqlite3_column_bytes(stmt, 2) + sqlite3_column_bytes(stmt,3) + 4;
        sender = sqlite3_column_text(stmt, 0);
        receiver = sqlite3_column_text(stmt, 1);
        time = sqlite3_column_text(stmt, 2);
        message = sqlite3_column_text(stmt, 3);
        msg = realloc(msg, length+6);
        int msg_size = sprintf(msg, "%s %s:%s %s", time, sender, receiver, message) + 1;

        notifs = api_msg_compose(code, msg_size, msg);
        api_send(api, notifs);

        sleep(0.1);
        free(notifs);
    }
    free(msg);
}

void load_users(struct api_state *api, sqlite3_stmt *stmt){
    int length = 0;
    const unsigned char *user;
    char* msg = malloc(0);
    union CODE code = {C_USERS};
    struct api_msg* userlist;

    while(sqlite3_step(stmt) == SQLITE_ROW){
        length = sqlite3_column_bytes(stmt, 0) + 1;
        user = sqlite3_column_text(stmt, 0);
        msg = realloc(msg, length+2);
        int msg_size = sprintf(msg, "%s", user) + 1;

        userlist = api_msg_compose(code, msg_size, msg);
        api_send(api, userlist);

        sleep(0.1);
        free(userlist);
    }
    free(msg);
}