#ifndef _UTIL_H_
#define _UTIL_H_

#include <netinet/in.h>

int lookup_host_ipv4(const char *hostname, struct in_addr *addr);
int max(int x, int y);
int parse_port(const char *str, uint16_t *port_p);
void handlespace(char *str);
int to_str_arr(char *line, size_t len, char ***arrayptr);

#endif /* defined(_UTIL_H_) */
