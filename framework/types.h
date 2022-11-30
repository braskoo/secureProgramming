#ifndef _TYPES_H_
#define _TYPES_H_

#include <unistd.h>
#include <sqlite3.h>
#include <stddef.h>

#define MSG_LEN_MAX 1024
#define TXT_LEN_MAX MSG_LEN_MAX - sizeof(struct api_msg)

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

// contiguous block
struct api_msg {
  /* TODO add information about message */
  enum COMMANDS command;
  ssize_t msg_size;
  char msg[];
};

struct ui_state {
  char *command;
  ssize_t msg_size;
  char *msg;
};

struct client_state {
  struct api_state api;
  int eof;
  struct ui_state ui;
  /* TODO client state variables go here */
};



#endif