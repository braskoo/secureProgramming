#ifndef _WORKER_H_
#define _WORKER_H_

#include "map.h"

__attribute__((noreturn))
void worker_start(int connfd, int server_fd, int worker_idx, struct map *users);

#endif /* !defined(_WORKER_H_) */
