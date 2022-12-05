#ifndef _WORKER_COMMANDS_H_
#define _WORKER_COMMANDS_H_

#include "types.h"

void reply_msg(struct api_state *api, int msg_size, char *msg, enum REPLIES reply_code);

#endif