#ifndef _API_H_
#define _API_H_

struct api_msg {
  /* TODO add information about message */
  enum COMMANDS command;
  char *msg;
  char *username;
  char *password;
  ssize_t msg_size;
};

struct api_state {
  int fd;
  /* TODO add required fields */
};


int api_recv(struct api_state *state, struct api_msg *msg);
void api_recv_free(struct api_msg *msg);

void api_state_free(struct api_state *state);
void api_state_init(struct api_state *state, int fd);

int api_send(struct api_state *state, struct api_msg *msg);
void api_send_free(struct api_msg *msg);

/* TODO add API calls to send messages to perform client-server interactions */

#endif /* defined(_API_H_) */
