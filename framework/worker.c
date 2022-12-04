#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#include "api.h"
#include "util.h"
#include "worker.h"
#include "workerutil.h"
#include "db.h"

struct worker_state {
  struct api_state api;
  int eof;
  int server_fd;  /* server <-> worker bidirectional notification channel */
  int server_eof;
  char *curruser;
  sqlite3 *db;
  /* TODO worker state variables go here */
};

void send_ack(struct api_state state){
  union CODE code = {R_ACK};
  struct api_msg *msg = api_msg_compose(code, sizeof("ACK"), "ACK");

  api_send(&state, msg);
}

/**
 * @brief Reads an incoming notification from the server and notifies
 *        the client.
 */
static int handle_s2w_notification(struct worker_state *state) {
  /* TODO implement the function */
 
  
  char *sql_stmt = (char*)malloc( (159 + 8 + 8) * sizeof(char) ); // 109 is the max length of a message, 8 is the max length of a username
  sprintf(sql_stmt, 
          "SELECT time || ' ' || sender || ':' || receiver || ' ' || message FROM Messages WHERE receiver='' OR receiver=\' @%s\' OR sender=\'%s\' ORDER BY Time DESC LIMIT 1",
          state->curruser, state->curruser);
  sqlite3_stmt *stmt;

  if(prepare_db(state->db, sql_stmt, &stmt) < 0) {
    return -1;
  }
  load_msgs(&state->api, stmt);
  sqlite3_finalize(stmt);
  return 0;
}

void get_chat_history(struct worker_state *state){
  
  char *sql_stmt = (char*)malloc( (134 + 8 + 8) *sizeof(char)); // 82 is the length of the sql statement, 8 is the length of the username
  printf("hier ook\n");
  sprintf(sql_stmt, 
        "SELECT time || ' ' || sender || ':' || receiver || ' ' || message FROM Messages WHERE (receiver='' OR receiver=\' @%s\' OR sender=\'%s\')", 
        state->curruser, state->curruser);
  sqlite3_stmt *stmt;
  printf("hier niet %s\n", sql_stmt);
  if(prepare_db(state->db, sql_stmt, &stmt) < 0) {
    return;
  }
  load_msgs(&state->api, stmt);
  sqlite3_finalize(stmt);
}


/**
 * @brief         Notifies server that the worker received a new message
 *                from the client.
 * @param state   Initialized worker state
 */
/* TODO call this function to notify other workers through server */
__attribute__((unused))
static int notify_workers(struct worker_state *state) {
  char buf = 0;
  ssize_t r;

  /* we only need to send something to notify the other workers,
   * data does not matter
   */
  r = write(state->server_fd, &buf, sizeof(buf));
  if (r < 0 && errno != EPIPE) {
    perror("error: write of server_fd failed");
    return -1;
  }
  return 0;
}

void reply_msg (struct api_state *api, int msg_size, char *msg, enum REPLIES reply_code){

  union CODE code = {reply_code};
  struct api_msg *reply = api_msg_compose(code, msg_size * sizeof(char), msg);
  api_send(api, reply);
  free(reply);
}

/**
 * @brief         Handles a message coming from client
 * @param state   Initialized worker state
 * @param msg     Message to handle
 */
static int execute_request(
  struct worker_state *state,
  const struct api_msg *msg) {


  //TODO handle different requests
  switch (msg->code.command) {
    case C_PRIVMSG: {
      if(!state->curruser){
        reply_msg(&state->api, 32, "User is not currently logged in", R_INVALID);
        break;
      }
      sqlite3_stmt *stmt;
      
      char *buf = (char*)malloc(msg->msg_size);
      memcpy(buf, msg->msg, msg->msg_size);
      char *privmsg = strchr(buf, ' ') + 1;
      char *receiver = strtok(buf, " ");

      char* sql_stmt = (char*)malloc( (47 + 8) * sizeof(char) ); 
      sprintf(sql_stmt, "SELECT username FROM Users WHERE username=\'%s\'", receiver+1);
      if(prepare_db(state->db, sql_stmt, &stmt) < 0) {
        free(sql_stmt);
        free(buf);
        sqlite3_finalize(stmt);
        return -1;
      }

      if(!check_users(&state->api, stmt)){
        reply_msg(&state->api, 23, "error: user not found", R_INVALID);
        break;
      }

      sql_stmt = realloc(sql_stmt, (75 + 8 + 8) * sizeof(char) + msg->msg_size); //added space for curruser, sender and receiver
      sprintf(sql_stmt, "INSERT INTO Messages (sender, receiver, message) VALUES(\'%s\', \' %s\', \'%s\')", state->curruser, 
              receiver, privmsg);
      
      if(exec_query(state->db, sql_stmt) < 0){
        free(buf);
        free(sql_stmt);
        sqlite3_finalize(stmt);
        return -1;
      }

      free(buf);
      free(sql_stmt);
      sqlite3_finalize(stmt);
      notify_workers(state);
      //send_ack(state->api);
      break;
    }
    case C_PUBMSG: {
      if(!state->curruser){
        reply_msg(&state->api, 40, "error: command not currently available", R_PUBMSG);
        break;
      }

      char *sql_stmt = (char*)malloc((72 * sizeof(char)) + msg->msg_size);
      // using 0 as receiver field to mark a public message, we can change this later 
      sprintf(sql_stmt, "INSERT INTO Messages (sender, receiver, message) VALUES(\'%s\', \'\', \'%s\')", state->curruser, msg->msg);
      
      //Missing error handling for exec

      if(exec_query(state->db, sql_stmt) < 0){
        free(sql_stmt);
        return -1;
      }

      free(sql_stmt);
      notify_workers(state);
      //send_ack(state->api);
      break;
    }
    case C_REGISTER: {
      if(state->curruser){
        reply_msg(&state->api, 40, "error: command not currently available", R_REGISTER);
        break;
      }
      // struct string_pair buf;
      char *buf = (char*)malloc(msg->msg_size);
      memcpy(buf, msg->msg, msg->msg_size);
      char *username = strtok(buf, " ");
      char *password = strtok(NULL, " ");
      
      sqlite3_stmt *stmt;
      char *sql_stmt = (char*)malloc((47 + 8) * sizeof(char));
      sprintf(sql_stmt, "SELECT username FROM Users WHERE username=\'%s\'", username);
      if(prepare_db(state->db, sql_stmt, &stmt) < 0) {
        free(sql_stmt);
        sqlite3_finalize(stmt);
        return -1;
      }
      
      char *checked_user = check_users(&state->api, stmt);
      if(checked_user && strcmp(checked_user, username) == 0){
        
        int msg_size = 30 + strlen(username);
        char* message = (char*)malloc( (strlen(username) + 30) * sizeof(char));
        sprintf(message, "error: user %s already exists", username);
        reply_msg(&state->api, msg_size + 1, message, R_INVALID);
        free(checked_user);
        goto cleanup;
        
      }
      sql_stmt = realloc(sql_stmt, (71 * sizeof(char)) + msg->msg_size);
      sprintf(sql_stmt, "INSERT INTO Users (username, password, status) VALUES(\'%s\', \'%s\', \'1\')", username, password);
      // worker_split_string(msg->msg, &buf);
      if(exec_query(state->db, sql_stmt) < 0){
        free(sql_stmt);
        return -1;
      }
      reply_msg(&state->api, 24, "registration succeeded", R_REGISTER);
      state->curruser = username;
      get_chat_history(state);

      cleanup:
      sqlite3_finalize(stmt);
      free(sql_stmt);
      break;
    }
    case C_USERS: {
      if(!state->curruser){
        reply_msg(&state->api, 40, "error: command not currently available", R_USERS);
        break;
      }
      sqlite3_stmt *stmt;
      char *sql_stmt = (char*)malloc(42 * sizeof(char));
      sprintf(sql_stmt, "SELECT username FROM Users WHERE status=1");
      if(prepare_db(state->db, sql_stmt, &stmt) < 0) {
        sqlite3_finalize(stmt);
        free(sql_stmt);
        return -1;
      }
      load_users(&state->api, stmt);
      
      free(sql_stmt);
      sqlite3_finalize(stmt);
      break;
    }
    default: {
      if(state->curruser){
        reply_msg(&state->api, 40, "error: command not currently available", R_LOGIN);
        break;
      }
      
      char *buf = (char*)malloc(msg->msg_size);
      memcpy(buf, msg->msg, msg->msg_size);
      char *username = strtok(buf, " ");
      char *password = strtok(NULL, " ");
      char *sql_stmt = (char*)malloc( (48 + 8) * sizeof(char));
      sprintf(sql_stmt, "SELECT password FROM Users WHERE username=\'%s\'", username);
      sqlite3_stmt *stmt;
      if(prepare_db(state->db, sql_stmt, &stmt) < 0) {
        free(sql_stmt);
        return -1;
      }

      if(sqlite3_step(stmt) != SQLITE_ROW){
        reply_msg(&state->api, 28, "error: invalid credentials", R_LOGIN);
      } else {
        if(strcmp(password, (char*)sqlite3_column_text(stmt, 0)) == 0){
          reply_msg(&state->api, 26, "authentication succeeded", R_LOGIN);
          state->curruser = username; //update current user of this worker
          sql_stmt = realloc(sql_stmt, (47 + 8) * sizeof(char)); //username length max 8, see pdf
          sprintf(sql_stmt, "UPDATE Users SET status=1 WHERE username=\'%s\'", username); // update database to show user as logged in
          if(exec_query(state->db, sql_stmt) < 0){
            free(sql_stmt);
            return -1;
          }
          printf("hij komt hier wel\n");
          get_chat_history(state);
        } else {
          reply_msg(&state->api, 28, "error: invalid credentials", R_LOGIN);
        }
      }
      free(sql_stmt);
      sqlite3_finalize(stmt);
      break;
    }
  }
  

  return 0;
}

/**
 * @brief         Reads an incoming request from the client and handles it.
 * @param state   Initialized worker state
 */
static int handle_client_request(struct worker_state *state) {
  int r, success = 1;
  assert(state);

  struct api_msg *msg = api_recv(&state->api);
  /* wait for incoming request, set eof if there are no more requests */
  r = msg->code.command;
  if (r < 0) return -1;
  if (r == 0) {
    state->eof = 1;
    return 0;
  }

  /* execute request */
  if (execute_request(state, msg) != 0) {
    success = 0;
  }

  /* clean up state associated with the message */
  free(msg);

  return success ? 0 : -1;
}

static int handle_s2w_read(struct worker_state *state) {
  char buf[256];
  ssize_t r;

  /* notification from the server that the workers must notify their clients
   * about new messages; these notifications are idempotent so the number
   * does not actually matter, nor does the data sent over the pipe
   */
  errno = 0;
  r = read(state->server_fd, buf, sizeof(buf));
  if (r < 0) {
    perror("error: read server_fd failed");
    return -1;
  }
  if (r == 0) {
    state->server_eof = 1;
    return 0;
  }

  /* notify our client */
  if (handle_s2w_notification(state) != 0) return -1;

  return 0;
}

/**
 * @brief Registers for: client request events, server notification
 *        events. In case of a client request, it processes the
 *        request and sends a response to client. In case of a server
 *        notification it notifies the client of all newly received
 *        messages.
 *
 */
static int handle_incoming(struct worker_state *state) {
  int fdmax, r, success = 1;
  fd_set readfds;

  assert(state);

  /* list file descriptors to wait for */
  FD_ZERO(&readfds);
  /* wake on incoming messages from client */
  FD_SET(state->api.fd, &readfds);
  /* wake on incoming server notifications */
  if (!state->server_eof) FD_SET(state->server_fd, &readfds);
  fdmax = max(state->api.fd, state->server_fd);

  /* wait for at least one to become ready */
  r = select(fdmax+1, &readfds, NULL, NULL, NULL);
  if (r < 0) {
    if (errno == EINTR) return 0;
    perror("error: select failed");
    return -1;
  }

  /* handle ready file descriptors */
  /* TODO once you implement encryption you may need to call ssl_has_data
   * here due to buffering (see ssl-nonblock example)
   */
  if (FD_ISSET(state->api.fd, &readfds)) {
    if (handle_client_request(state) != 0) success = 0;
  }
  if (FD_ISSET(state->server_fd, &readfds)) {
    if (handle_s2w_read(state) != 0) success = 0;
  }
  return success ? 0 : -1;
}

/**
 * @brief Initialize struct worker_state before starting processing requests.
 * @param state        worker state
 * @param connfd       connection file descriptor
 * @param pipefd_w2s   pipe to notify server (write something to notify)
 * @param pipefd_s2w   pipe to be notified by server (can read when notified)
 *
 */
static int worker_state_init(
  struct worker_state *state,
  int connfd,
  int server_fd,
  int worker_idx) {

  /* initialize */
  memset(state, 0, sizeof(*state));
  state->server_fd = server_fd;
  state->curruser = NULL;
  /* set up API state */
  api_state_init(&state->api, connfd);

  sqlite3 *db;
  
  int rc = sqlite3_open("chat.db", &db);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open db, %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    
    return 1;
  }
  state->db = db;
  return 0;
}

/**
 * @brief Clean up struct worker_state when shutting down.
 * @param state        worker state
 *
 */
static void worker_state_free(
  struct worker_state *state) {
  /* TODO any additional worker state cleanup */

  /* clean up API state */
  api_state_free(&state->api);

  /* close file descriptors */
  close(state->server_fd);
  close(state->api.fd);
  sqlite3_close(state->db);
}

/**
 * @brief              Worker entry point. Called by the server when a
 *                     worker is spawned.
 * @param connfd       File descriptor for connection socket
 * @param pipefd_w2s   File descriptor for pipe to send notifications
 *                     from worker to server
 * @param pipefd_s2w   File descriptor for pipe to send notifications
 *                     from server to worker
 */
__attribute__((noreturn))
void worker_start(
  int connfd,
  int server_fd, 
  int worker_idx) {
  struct worker_state state;
  int success = 1;

  /* initialize worker state */
  if (worker_state_init(&state, connfd, server_fd, worker_idx) != 0) {
    goto cleanup;
  }
  /* TODO any additional worker initialization */

  /* handle for incoming requests */
  while (!state.eof) {
    if (handle_incoming(&state) != 0) {
      success = 0;
      break;
    }
  }

cleanup:
  /* cleanup worker */
  /* TODO any additional worker cleanup */
  worker_state_free(&state);

  exit(success ? 0 : 1);
}
