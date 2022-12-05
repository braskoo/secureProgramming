#ifndef _UI_H_
#define _UI_H_

#include "types.h"

void ui_state_free(struct ui_state *state);
void ui_state_fill(const char *line, size_t len, struct ui_state *buf);
void ui_state_init(struct ui_state *state);
enum COMMANDS ui_command_parse(struct ui_state *state);

/* TODO add UI calls interact with user on stdin/stdout */

#endif /* defined(_UI_H_) */
