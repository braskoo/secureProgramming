#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ui.h"
#include "util.h"

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
  if(msg - 1) state->msg_size = strlen(msg);
  else state->msg_size = 0;
}

void ui_state_parse(struct ui_state *state, struct api_msg *buf){
    assert(buf);

    // parse command type
    if(!state->command){ 
      if(state->msg[0] == '@') buf->command = C_PRIVMSG;
      else buf->command = C_PUBMSG;
    }
    else if(strcmp(state->command, "/exit\n") == 0) exit(0);
    else if(strcmp(state->command, "/register") == 0) buf->command = C_REGISTER;
    else if(strcmp(state->command, "/users\n") == 0) buf->command = C_USERS;
    else if(strcmp(state->command, "/login") == 0) buf->command = C_LOGIN;
  
    buf->msg = state->msg;
    buf->msg_size = strlen(state->msg);
}
