#include <assert.h>

#include "ui.h"

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
  buf = realloc(buf, sizeof(struct ui_state));
}

void ui_state_fill(char *line, ssize_t size, struct ui_state *state){
  char *command, *msg;
  char curr;

  for(int i = 0; curr != ' ' && curr != NULL; i++){
    curr = line[i];
  }

  
}

void ui_state_parse(struct ui_state *state, struct api_msg *buf){
    if (state->command[0] == '@'){
      printf("privatemsgcommand\n");
    } else if (state->command[0] == '/') {
      char* token = state->command;
      if(strcmp(token, "/register") == 0){
        printf("registration succeeded\n");
      } else if(strcmp(token, "/users\n") == 0){
        printf("no users atm\n");
      } else if(strcmp(token, "/exit\n") == 0){
        exit(0);
      } else if(strcmp(token, "/login") == 0){
        printf("login succeeded\n");
      } else {
        printf("unknown command\n");
      }
    } else {
      
    }
}
