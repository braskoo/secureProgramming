#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>

#include "workerutil.h"
#include "api.h"
#include "types.h"

void reply_msg(struct api_state *api, int msg_size, char *msg, enum REPLIES reply_code)
{
    union CODE code = {reply_code};
    struct api_msg *reply = api_msg_compose(code, msg_size * sizeof(char), msg);
    api_send(api, reply);
    free(reply);
}

