#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "workerutil.h"

// splits string into two parts UNUSED
void worker_split_string(const char line[]){
    char *split_idx = strchr(line, ' ');

    printf("split | first: (%s), second: (%s)\n", line, split_idx + 1);
}