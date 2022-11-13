#ifndef _WORKERUTIL_H_
#define _WORKERUTIL_H_

struct string_pair{
    char *first;
    char *second;
};

void worker_split_string(char *line, struct string_pair *buf);

#endif