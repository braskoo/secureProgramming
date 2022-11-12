#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "map.h"

void map_delete(struct map *map, int idx){
    assert(map);
    map->users[idx].username = NULL;
}

void map_init(struct map* map){
    map->users = (struct user*)calloc(16, sizeof(struct user));
}

void map_set(struct map *map, struct user user, int idx){
    assert(map);
    map->users[idx] = user;
}

void map_getusers(struct map *map, char *buf){
    // 8 strings max of 8 + 1 character each, and one null terminator
    buf = (char*)malloc(16 * 9 + 1);
    assert(map);

    for(int i = 0; i < 16; i++){
        struct user *current = map->users + i;

        if(current->username){
            strcat(buf, ' ' + current->username);
        }
    }
}

int map_getfd(struct map *map, char *username){
    assert(map && username);
    for(int i = 0; i < 16; i++){
        struct user *current = map->users + i;

        if(current->username && strcmp(current->username, username) == 0){
            return current->fd;
        }
    }

    return -1;
}