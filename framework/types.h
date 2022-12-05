#ifndef _TYPES_H_
#define _TYPES_H_

#include <unistd.h>
#include <sqlite3.h>
#include <stddef.h>

#define MSG_LEN_MAX 1024
#define TXT_LEN_MAX MSG_LEN_MAX - sizeof(struct api_msg)

enum COMMANDS
{
  C_INVALID,
  C_PRIVMSG,
  C_PUBMSG,
  C_REGISTER,
  C_USERS,
  C_LOGIN,
  C_EXIT,
  C_UNKNOWN
};

enum REPLIES
{
  R_SOCKERR = -1,
  R_SOCKCLOSED = 0,
  R_ACK,
  R_PRIVMSG,
  R_PUBMSG,
  R_LOGIN,
  R_REGISTER,
  R_INVALID,
  R_USERS
};

union CODE
{
  enum REPLIES reply;
  enum COMMANDS command;
};

struct api_state
{
  int fd;
  /* TODO add required fields */
};

// contiguous block
struct api_msg
{
  /* TODO add information about message */
  union CODE code;
  ssize_t msg_size;
  char msg[];
};

struct ui_state
{
  ssize_t msg_size;
  char **str_arr;
  int argc;
  char *msg;
};

struct client_state
{
  struct api_state api;
  int eof;
  struct ui_state ui;
  /* TODO client state variables go here */
};

struct worker_state
{
  struct api_state api;
  int eof;
  int server_fd; /* server <-> worker bidirectional notification channel */
  int server_eof;
  char *curruser;
  sqlite3 *db;
  /* TODO worker state variables go here */
};

#endif