#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

#include "api.h"

void api_debug_msg(const struct api_msg *msg, const char* str){
  int code = 0;

  if(strncmp(str, "RECV", sizeof("RECV"))){
    code = msg->code.command;
  } else {
    code = msg->code.reply;
  }

  printf("%s, api msg: %x|%li|%s\n", str, code, msg->msg_size, msg->msg_size > 0 ? msg->msg : "");
}

/**
 * @brief         Receive the next message from the sender and allocates new api_msg in heap memory as buffer
 * @param state   Initialized API state
 * @return        Returns pointer to newly allocated api_msg block. command contains the return value of recv
 */
struct api_msg *api_recv(struct api_state *state){
  assert(state);

  struct api_msg *msg = malloc(MSG_LEN_MAX);

  int recieved = recv(state->fd, msg, MSG_LEN_MAX, 0);

  if(recieved <= 0){
    msg->code.command = recieved;
    msg = realloc(msg, sizeof(struct api_msg));
    msg->msg_size = 0;
  } else {
    msg = realloc(msg, recieved);
    msg->msg_size = recieved - sizeof(struct api_msg);
  }
  
  api_debug_msg(msg, "RECV");

  return msg;
}

// size has to include null terminator
struct api_msg *api_msg_compose(union CODE code, ssize_t size, const char *text){
  assert(text);

  struct api_msg *msg = malloc(size + sizeof(struct api_msg));

  msg->code = code;
  msg->msg_size = size;
  strncpy(msg->msg, text, size);

  return msg;
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

ssize_t api_send(struct api_state *api, const struct api_msg *request){
  api_debug_msg(request, "SEND");

  ssize_t sent = send(api->fd, request, request->msg_size + sizeof(struct api_msg), MSG_DONTWAIT);

  return sent;
}