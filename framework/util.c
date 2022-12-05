#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "util.h"

int lookup_host_ipv4(const char *hostname, struct in_addr *addr)
{
  struct hostent *host;

  assert(hostname);
  assert(addr);

  /* look up hostname, find first IPv4 entry */
  host = gethostbyname(hostname);
  while (host)
  {
    if (host->h_addrtype == AF_INET &&
        host->h_addr_list &&
        host->h_addr_list[0])
    {
      assert(host->h_length == sizeof(*addr));
      memcpy(addr, host->h_addr_list[0], sizeof(*addr));
      return 0;
    }
    host = gethostent();
  }

  fprintf(stderr, "error: unknown host: %s\n", hostname);
  return -1;
}

int max(int x, int y)
{
  return (x > y) ? x : y;
}

int parse_port(const char *str, uint16_t *port_p)
{
  char *endptr;
  long value;

  assert(str);
  assert(port_p);

  /* convert string to number */
  errno = 0;
  value = strtol(str, &endptr, 0);
  if (!value && errno)
    return -1;
  if (*endptr)
    return -1;

  /* is it a valid port number */
  if (value < 0 || value > 65535)
    return -1;

  *port_p = value;
  return 0;
}

void handlespace(char *str)
{
  int index = 0, i;

  while (str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
  {
    index++;
  }

  if (index != 0)
  {
    i = 0;
    while (str[i + index] != '\0')
    {
      str[i] = str[i + index];
      i++;
    }
    str[i] = '\0';
  }
}

// places pointers to all words in line into arrayptr. returns the count of entries
int to_str_arr(char *line, size_t len, char ***arrayptr)
{
  int count = 0;
  char **arr;

  for (char *curr = line; curr < line + len; curr++)
  {
    if (curr && !isspace(*curr))
    {
      count++;                                    // one extra arg found
      arr = realloc(arr, count * sizeof(char *)); // one extra space for the new string pointer
      arr[count - 1] = curr;                      // pointer to string stored in array
      curr = strchr(curr, ' ');                   // increment counter to end of string so we dont check another character of this string again
      if (curr)
        *curr = '\0';
      else
        break; // null terminate current string, break if strchr returned null (no more words in the string)
    }
  }

  *arrayptr = arr;

  // arrptr is now a pointer to the string array (heap memory so not out of scope)
  // that string array contains the pointers to the individual strings inside of line

  return count;
}