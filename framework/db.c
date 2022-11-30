
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int prepare_db(sqlite3 **db, char *sql_stmt, sqlite3_stmt **ppStmt) {
  int rc;
  char *err_msg = 0;
  char *sql;

  int rc = sqlite3_prepare_v2(db, sql_stmt, -1, ppStmt, NULL);
  if (rc != SQLITE_OK ) {
    printf("error: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return -1;
  }
  return 0;
}