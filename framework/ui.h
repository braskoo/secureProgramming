#ifndef _UI_H_
#define _UI_H_

enum COMMANDS{
  C_PRIVMSG, 
  C_PUBMSG,
  C_REGISTER,
  C_USERS,
  C_LOGIN
};

struct ui_state {
  char *command;
  char *msg;
  ssize_t msg_size;
};

void ui_state_free(struct ui_state *state);
void ui_state_fill(char *line, ssize_t size, struct ui_state *buf);
void ui_state_init(struct ui_state *state);
void ui_state_parse(struct ui_state *state, struct api_msg *buf);


/* TODO add UI calls interact with user on stdin/stdout */

#endif /* defined(_UI_H_) */
