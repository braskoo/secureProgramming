#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ui.h"
#include "util.h"
#include "string.h"

/**
 * @brief         Frees ui_state context
 * @param state   Initialized UI state to be cleaned up
 */
void ui_state_free(struct ui_state *state) {

  assert(state);

  /* TODO free ui_state */
}

/**
 * @brief         Initializes ui_state context
 * @param state   UI state to be initialized
 */
void ui_state_init(struct ui_state *buf){
  assert(buf);
}

void ui_state_fill(char *line, struct ui_state *state){
  char* command = NULL, *msg;

  // get command if there is one
  if(line[0] == '/'){
    // get start of message before strtok screws up line
    msg = strchr(line, ' ') + 1;
    command = strtok(line, " ");
  } else {
    // if there is no "real" command, we just pass forward command and let the server handle it
    msg = line;
  }

  // fill ui state
  state->command = command;
  state->msg = msg;

  // we check if strchr returned NULL
  state->msg_size = (msg - 1) ? strlen(msg) : 0;
}

// allocates and fills a new api message in heap memory
struct api_msg *ui_state_parse(struct ui_state *state){
  struct api_msg* ptr = malloc(state->msg_size + sizeof(struct api_msg));

  // parse command type
  if(!state->command){ 
    if(state->msg[0] == '@') ptr->code.command = C_PRIVMSG;
    else ptr->code.command = C_PUBMSG;
  }
  else if(strcmp(state->command, "/exit\n") == 0) ptr->code.command = C_EXIT;
  else if(strcmp(state->command, "/register") == 0) ptr->code.command = C_REGISTER;
  else if(strcmp(state->command, "/login") == 0) ptr->code.command = C_LOGIN;
  else if(strcmp(state->command, "/users\n") == 0) ptr->code.command = C_USERS;
  else ptr->code.command = C_INVALID;
  
  if(ptr->code.command != C_INVALID && state->msg_size > 0){
    strncpy(ptr->msg, state->msg, state->msg_size);
    ptr->msg_size = state->msg_size;
  } else ptr->msg_size = 0;

  return ptr;
}
