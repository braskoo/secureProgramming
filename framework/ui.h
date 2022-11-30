#ifndef _UI_H_
#define _UI_H_

#include "types.h"

void ui_state_free(struct ui_state *state);
void ui_state_fill(char *line, struct ui_state *buf);
void ui_state_init(struct ui_state *state);
int ui_state_parse(struct ui_state *state, struct api_msg *buf);
int ui_input_validate(char *line);

/* TODO add UI calls interact with user on stdin/stdout */

#endif /* defined(_UI_H_) */
