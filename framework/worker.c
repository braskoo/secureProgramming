#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3>

#include "api.h"
#include "util.h"
#include "worker.h"
#include "map.h"
#include "workerutil.h"

struct worker_state {
  struct api_state api;
  int eof;
  int server_fd;  /* server <-> worker bidirectional notification channel */
  int server_eof;
  int worker_idx;
  struct map *users;
  

  /* TODO worker state variables go here */
};

/**
 * @brief Reads an incoming notification from the server and notifies
 *        the client.
 */
static int handle_s2w_notification(struct worker_state *state) {
  /* TODO implement the function */
  return -1;
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

/**
 * @brief         Handles a message coming from client
 * @param state   Initialized worker state
 * @param msg     Message to handle
 */
static int execute_request(
  struct worker_state *state,
  const struct api_msg *msg) {

  printf("worker recieved: %i | %s\n", msg->command, msg->msg);

  //TODO handle different requests
  switch (msg->command) {
    case C_PRIVMSG: {
      struct string_pair buf;
      worker_split_string(msg->msg, &buf);
      int target_fd = map_getfd(state->users, buf.first + 1);

      if(target_fd >= 0){
        send(target_fd, msg->msg, msg->msg_size, 0);
      }

      char *sql_insert = (char*)malloc(1200 * sizeof(char));


      // sender fd not defined, should it be part of api_message?
      // sprintf(sql_insert, "INSERT INTO (Messages) VALUES(\'%d\', \'%d\', DEFAULT, \'%s\');",sender_fd, target_fd, msg->msg);

      //Missing error handling for exec
      // sqlite3_exec(state->api->db, sql_insert, 0, 0, &err_msg);
      

      break;
    }
    case C_PUBMSG: {
      int *fd_all = malloc(MAX_CHILDREN * sizeof(int));
      map_getfds_all(state->users, fd_all);

      for(int i = 0; i < MAX_CHILDREN; i++){
        if(fd_all[i] >= 0){
          send(fd_all[i], msg->msg, msg->msg_size, 0);
        }

      char *sql_insert = (char*)malloc(1200 * sizeof(char));

      // sender_fd needed
      // using all as receiver field to mark a public message, we can change this later 

      // sprintf(sql_insert, "INSERT INTO (Messages) VALUES(\'%d\', \'all\', DEFAULT, \'%s\');",sender_fd, msg->msg);

      //Missing error handling for exec
      // sqlite3_exec(state->api->db, sql_insert, 0, 0, &err_msg);
      
      }
      break;
    }
    case C_REGISTER: {
      printf("processing register?\n");
      struct user new_user;
      struct string_pair buf;
      printf("string split?\n");
      worker_split_string(msg->msg, &buf);
      printf("string split!\n");
      new_user.username = buf.first;
      new_user.fd = state->api.fd;

      printf("setting map\n");
      map_set(state->users, new_user, state->worker_idx);
      printf("fd %i registered with name (%s)", new_user.fd, new_user.username);
      send(state->api.fd, "REGISTERED", 11, 0);
      break;
    }
    case C_USERS: {
      send(state->api.fd, "0THERE ARE NO USERS YET", 24, 0);
      break;
    }
    default: 
      send(state->api.fd, "0YOU CANT LOGIN YET", 20, 0);
      break;
  }
  

  return 0;
}

/**
 * @brief         Reads an incoming request from the client and handles it.
 * @param state   Initialized worker state
 */
static int handle_client_request(struct worker_state *state) {
  struct api_msg msg;
  int r, success = 1;

  printf("worker recieving data\n");

  assert(state);

  /* wait for incoming request, set eof if there are no more requests */
  r = api_recv(&state->api, &msg);
  if (r < 0) return -1;
  if (r == 0) {
    state->eof = 1;
    return 0;
  }

  printf("worker recieved valid amount of data\n");

  /* execute request */
  if (execute_request(state, &msg) != 0) {
    success = 0;
  }

  /* clean up state associated with the message */
  api_recv_free(&msg);

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
  int worker_idx, 
  struct map *users) {

  /* initialize */
  memset(state, 0, sizeof(*state));
  state->server_fd = server_fd;

  /* set up API state */
  api_state_init(&state->api, connfd);

  state->users = users;

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
  int worker_idx, 
  struct map *users) {
  struct worker_state state;
  int success = 1;

  /* initialize worker state */
  if (worker_state_init(&state, connfd, server_fd, worker_idx, users) != 0) {
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
