#ifndef _TYPES_H_
#define _TYPES_H_

#include <unistd.h>

enum COMMANDS{
  C_PRIVMSG, 
  C_PUBMSG,
  C_REGISTER,
  C_USERS,
  C_LOGIN
};

struct api_state {
  int fd;
  /* TODO add required fields */
};

struct api_msg {
  /* TODO add information about message */
  enum COMMANDS command;
  char *msg;
  ssize_t msg_size;
};

struct ui_state {
  char *command;
  char *msg;
  ssize_t msg_size;
};

struct client_state {
  struct api_state api;
  int eof;
  struct ui_state ui;
  /* TODO client state variables go here */
};



#endif