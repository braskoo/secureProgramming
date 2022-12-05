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
void ui_state_free(struct ui_state *state)
{

  assert(state);

  /* TODO free ui_state */
}

/**
 * @brief         Initializes ui_state context
 * @param state   UI state to be initialized
 */
void ui_state_init(struct ui_state *buf)
{
  assert(buf);
}

void ui_state_fill(char *line, struct ui_state *state)
{
  char *command = NULL, *msg;

  // get command if there is one
  if (line[0] == '/')
  {
    // get start of message before strtok screws up line
    msg = strchr(line, ' ') + 1;
    command = strtok(line, " ");
  }
  else
  {
    // if there is no "real" command, we just pass forward command and let the server handle it
    msg = line;
  }

  // fill ui state
  state->command = command;
  state->msg = msg;
  // we check if strchr returned NULL
  state->msg_size = (msg - 1) ? strlen(msg) + 1 : 0;
}

// parses command
enum COMMANDS ui_command_parse(struct ui_state *state)
{
  enum COMMANDS command;
  char **str_arr = NULL;

  // parse command type
  if (!state->command)
  {
    if (state->msg[0] == '@')
      command = C_PRIVMSG;
    else
      command = C_PUBMSG;
  }
  else if (strcmp(state->command, "/exit") == 0)
  {
    if (!state->msg)
      command = C_INVALID;
    else
      command = C_EXIT;
  }
  else if (strcmp(state->command, "/register") == 0)
  {
    int argc = to_str_arr(state->msg, state->msg_size, &str_arr);
    if (argc == 2)
    {
      command = C_REGISTER;
      state->msg_size = sprintf(state->msg, "%s %s", str_arr[0], str_arr[1]) + 1;
    }
    else
      command = C_INVALID;
  }
  else if (strcmp(state->command, "/login") == 0)
  {
    int argc = to_str_arr(state->msg, state->msg_size, &str_arr);
    printf("%d\n", argc);
    if (argc == 2)
    {
      command = C_LOGIN;
      state->msg_size = sprintf(state->msg, "%s %s", str_arr[0], str_arr[1]) + 1;
    }
    else
      command = C_INVALID;
  }
  else if (strcmp(state->command, "/users") == 0)
  {
    if (!state->msg)
      command = C_INVALID;
    else
      command = C_USERS;
  }
  else
    command = C_UNKNOWN;

  free(str_arr);

  return command;
}
