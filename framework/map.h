#ifndef _MAP_H_
#define _MAP_H_

#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct user {
    char *username;
    int fd;
};

struct map
{
    struct user *users;
};

void map_delete(struct map *map, int idx);
void map_init(struct map* map);
void map_set(struct map *map, struct user user, int idx)
void map_getusers(struct map *map, char *buf);
int map_getfd(struct map *map, char *username);

#endif