#ifndef _API_H_
#define _API_H_

#include "types.h"

void api_debug_msg(const struct api_msg *msg, const char* str);

struct api_msg *api_recv(struct api_state *state);
void api_recv_free(struct api_msg *msg);

void api_state_free(struct api_state *state);
void api_state_init(struct api_state *state, int fd);

ssize_t api_send(struct api_state *state, const struct api_msg *msg);
void api_send_free(struct api_msg *msg);

/* TODO add API calls to send messages to perform client-server interactions */

#endif /* defined(_API_H_) */
