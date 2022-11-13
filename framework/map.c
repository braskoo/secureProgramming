#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "map.h"

void map_delete(struct map *map, int idx){
    assert(map);
    free(map->users[idx].username);
    map->users[idx].username = NULL;
}

void map_init(struct map* map){
    map->users = (struct user*)calloc(MAX_CHILDREN, sizeof(struct user));
}

void map_set(struct map *map, struct user user, int idx){
    printf("we in map set bruv\n");
    printf("trying to set worker %i's client to fd: %i, username: (%s)\n", idx, user.fd, user.username);
    assert(map);
    map->users[idx] = user;
}

void map_getusers(struct map *map, char *buf){
    // 8 strings max of 8 + 1 character each, and one null terminator
    buf = (char*)malloc(MAX_CHILDREN * 9 + 1);
    assert(map);

    for(int i = 0; i < MAX_CHILDREN; i++){
        struct user *current = map->users + i;

        if(current->username){
            strcat(buf, ' ' + current->username);
        }
    }
}

int map_getfd(struct map *map, char *username){
    assert(map && username);
    for(int i = 0; i < MAX_CHILDREN; i++){
        struct user *current = map->users + i;

        if(current->username && strcmp(current->username, username) == 0){
            return current->fd;
        }
    }

    return -1;
}

void map_getfds_all(struct map *map, int *buf){
    assert(map && buf);

    for(int i = 0; i < MAX_CHILDREN; i++){
        struct user current = map->users[i];
        buf[i] = (current.username) ? current.fd : -1;
    }
}