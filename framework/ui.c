#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "ui.h"
#include "util.h"
#include "string.h"

/**
 * @brief         Frees ui_state context
 * @param state   Initialized UI state to be cleaned up
 */
void ui_state_free(struct ui_state *state)
{
  assert(state);

  free(state->msg);
  for(int i = 0; i < state->argc; i++){
    free(state->str_arr[i]);
  }

  state->argc = 0;
}

/**
 * @brief         Initializes ui_state context
 * @param state   UI state to be initialized
 */
void ui_state_init(struct ui_state *buf)
{
  assert(buf);
}

// places pointers to duplicated words in line into arrayptr
void get_str_arr(const char *line, size_t len, struct ui_state *state)
{
  state->argc = 0;

  for(int i = 0; i < len; i++){
    if(!isspace(line[i])){
      state->argc++;
      state->str_arr = realloc(state->str_arr, sizeof(char*) * state->argc);
      state->str_arr[state->argc - 1] = strndup(line + i, get_arg_len(line + i));
      while(!isspace(line[i])) i++;
    }
  }
}

void ui_state_fill(const char *line, size_t len, struct ui_state *state)
{
  get_str_arr(line, len, state);
  state->msg = "\0"; // default value so we don't get invalid frees
}

// parses command and stores all arguments in state->args.msg
enum COMMANDS ui_command_parse(struct ui_state *state)
{
  enum COMMANDS command_code;
  char *command_arg = state->str_arr[0];

  // parse command type
  if (state->str_arr[0][0] != '/')
  {
    if (state->str_arr[0][0] == '@')
    {
      command_code = C_PRIVMSG;
    }
    else
    {
      command_code = C_PUBMSG;
    }
  }
  else if (strcmp(command_arg, "/exit") == 0)
  {
    if (state->argc > 1)
      command_code = C_INVALID;
    else
      command_code = C_EXIT;
  }
  else if (strcmp(command_arg, "/register") == 0)
  {
    if (state->argc == 3)
    {
      command_code = C_REGISTER;
    }
    else
      command_code = C_INVALID;
  }
  else if (strcmp(command_arg, "/login") == 0)
  {
    if (state->argc == 3)
    {
      command_code = C_LOGIN;
    }
    else
      command_code = C_INVALID;
  }
  else if (strcmp(command_arg, "/users") == 0)
  {
    if (state->argc > 1)
    {
      command_code = C_INVALID;
    }
    else
      command_code = C_USERS;
  }
  else
  {
    command_code = C_INVALID;
  }

  return command_code;
}
