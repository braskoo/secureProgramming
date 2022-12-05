#ifndef _WORKER_COMMANDS_H_
#define _WORKER_COMMANDS_H_

#include "types.h"

void reply_msg(struct api_state *api, int msg_size, char *msg, enum REPLIES reply_code);
void split_msg(const char *msg, ssize_t size, char **buf, char **username, char **password);
int logged(struct api_state *api, char *user);

#endif