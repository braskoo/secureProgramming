#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

#include "api.h"

/**
 * @brief         Receive the next message from the sender and stored in @msg
 * @param state   Initialized API state
 * @param msg     Information about message is stored here
 * @return        Returns 1 on new message, 0 in case socket was closed,
 *                or -1 in case of error.
 */
int api_recv(struct api_state *state, struct api_msg *msg) {
  assert(state);
  assert(msg);

  msg = realloc(msg, MSG_LEN_MAX);

  int recieved = recv(state->fd, msg, MSG_LEN_MAX, 0);

  if(recieved == -1){
    printf("error: unexpected error while recieving from socket\n");
    free(msg);
    return -1;
  }

  if(recieved == 0){
    printf("error: socket closed\n");
    free(msg);
    return -1;
  }

  msg = realloc(msg, recieved);
  msg->msg_size = recieved - sizeof(struct api_msg);

  return 1;
}

/**
 * @brief         Clean up information stored in @msg
 * @param msg     Information about message to be cleaned up
 */
void api_recv_free(struct api_msg *msg) {

  assert(msg);

  /* TODO clean up state allocated for msg */

}

/**
 * @brief         Frees api_state context
 * @param state   Initialized API state to be cleaned up
 */
void api_state_free(struct api_state *state) {

  assert(state);

  /* TODO clean up API state */
}

/**
 * @brief         Initializes api_state context
 * @param state   API state to be initialized
 * @param fd      File descriptor of connection socket
 */
void api_state_init(struct api_state *state, int fd) {

  assert(state);

  /* initialize to zero */
  memset(state, 0, sizeof(*state));

  /* store connection socket */
  state->fd = fd;

  /* TODO initialize API state */

}

ssize_t api_send(struct api_state *api, struct api_msg *request){
  char* buf = malloc(1 + request->msg_size);
  buf[0] = (char)request->command; 
  memcpy(buf + 1, request->msg, request->msg_size);

  ssize_t sent = send(api->fd, buf, request->msg_size + 1, MSG_DONTWAIT);

  return sent;
}