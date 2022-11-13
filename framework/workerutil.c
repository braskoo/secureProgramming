#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "workerutil.h"

void worker_split_string(char *line, struct string_pair *buf){
    char *split_idx = strchr(line, ' ');
    *split_idx = '\0';
    buf->first = line;
    buf->second = split_idx + 1;

    printf("split | first: (%s), second: (%s)\n", buf->first, buf->second);
}