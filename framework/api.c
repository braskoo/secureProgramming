#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

#include "api.h"

void api_debug_msg(const struct api_msg *msg, const char* str){
  printf("%s, api msg: %x|%li|%s\n", str, msg->command, msg->msg_size, msg->msg);
  
  // uint8_t *bytes = (uint8_t *)msg;
  // for(int i = 0; i < msg->msg_size + sizeof(struct api_msg); i++){
  //   printf("%02x ", bytes[i]);
  // }
  printf("\n");
}

/**
 * @brief         Receive the next message from the sender and allocates new api_msg in heap memory as buffer
 * @param state   Initialized API state
 * @return        Returns pointer to newly allocated api_msg block. command contains the return value of recv
 */
struct api_msg *api_recv(struct api_state *state){
  assert(state);

  struct api_msg * msg = malloc(MSG_LEN_MAX);

  int recieved = recv(state->fd, msg, MSG_LEN_MAX, 0);

  api_debug_msg(msg, "RECV");

  if(recieved <= 0){
    printf("socket error");
    msg->command = recieved;
  }

  msg = realloc(msg, recieved);
  msg->msg_size = recieved - sizeof(struct api_msg);

  return msg;
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

ssize_t api_send(struct api_state *api, const struct api_msg *request){
  api_debug_msg(request, "SEND");

  ssize_t sent = send(api->fd, request, request->msg_size + sizeof(struct api_msg), MSG_DONTWAIT);

  return sent;
}