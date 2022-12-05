#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "types.h"
#include "api.h"
#include "ui.h"
#include "util.h"

/**
 * @brief Connects to @hostname on port @port and returns the
 *        connection fd. Fails with -1.
 */
static int client_connect(struct client_state *state,
                          const char *hostname, uint16_t port)
{
  int fd;
  struct sockaddr_in addr;

  assert(state);
  assert(hostname);

  /* look up hostname */
  if (lookup_host_ipv4(hostname, &addr.sin_addr) != 0)
    return -1;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  /* create TCP socket */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    perror("error: cannot allocate server socket");
    return -1;
  }

  /* connect to server */
  if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
  {
    perror("error: cannot connect to server");
    close(fd);
    return -1;
  }

  return fd;
}

static int client_process_command(struct client_state *state)
{
  assert(state);

  int ret;

  setvbuf(stdout, NULL, _IONBF, 0);
  char *line = NULL;
  size_t len = 0;

  if (getline(&line, &len, stdin) == -1)
  {
    state->eof = 1;
    free(line);
    return -1;
  }

  // null terminate string
  line[strlen(line) - 1] = '\0';
  handlespace(line);
  // fill ui state with appropriate information from line and validate input
  ui_state_fill(line, &state->ui);

  enum COMMANDS command = ui_command_parse(&state->ui);
  union CODE code = {command};

  if (command == C_EXIT)
  {
    ret = -1;
    goto cleanup;
  }

  if (command == C_INVALID)
  {
    printf("error: invalid command format\n");
    ret = 0;
    goto cleanup;
  }

  if (command == C_UNKNOWN)
  {
    printf("error: unknown command %s\n", state->ui.command);
    ret = 0;
    goto cleanup;
  }

  struct api_msg *request = api_msg_compose(code, state->ui.msg_size, state->ui.msg);
  ssize_t sent = api_send(&state->api, request);

  if (sent == -1)
  {
    perror("socket closed");
    printf("hihi ur socket sucks\n");
    ret = -1;
  }

  ret = 0;
  free(request);

cleanup:
  free(line);
  return ret;
}

/**
 * @brief         Handles a message coming from server (i.e, worker)
 * @param state   Initialized client state
 * @param msg     Message to handle
 */
static int execute_request(struct client_state *state, const struct api_msg *msg)
{
  printf("%s\n", msg->msg);

  return 0;
}

/**
 * @brief         Reads an incoming request from the server and handles it.
 * @param state   Initialized client state
 */
static int handle_server_request(struct client_state *state)
{
  int r, success = 1;

  assert(state);

  struct api_msg *msg = api_recv(&state->api);
  /* wait for incoming request, set eof if there are no more requests */
  r = msg->code.command;
  if (r < 0)
    return -1;
  if (r == 0)
  {
    state->eof = 1;
    return 0;
  }

  /* execute request */
  if (execute_request(state, msg) != 0)
  {
    success = 0;
  }

  /* clean up state associated with the message */
  free(msg);

  return success ? 0 : -1;
}

/**
 * @brief register for multiple IO event, process one
 *        and return. Returns 0 if the event was processed
 *        successfully and -1 otherwise.
 *
 */
static int handle_incoming(struct client_state *state)
{
  int fdmax, r;
  fd_set readfds;

  assert(state);

  /* TODO if we have work queued up, this might be a good time to do it */

  /* TODO ask user for input if needed */

  /* list file descriptors to wait for */
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);
  FD_SET(state->api.fd, &readfds);
  fdmax = state->api.fd;

  /* wait for at least one to become ready */
  r = select(fdmax + 1, &readfds, NULL, NULL, NULL);
  if (r < 0)
  {
    if (errno == EINTR)
      return 0;
    perror("error: select failed");
    return -1;
  }

  /* handle ready file descriptors */
  if (FD_ISSET(STDIN_FILENO, &readfds))
  {
    return client_process_command(state);
  }
  /* TODO once you implement encryption you may need to call ssl_has_data
   * here due to buffering (see ssl-nonblock example)
   */
  if (FD_ISSET(state->api.fd, &readfds))
  {
    return handle_server_request(state);
  }
  return 0;
}

static int client_state_init(struct client_state *state)
{
  /* clear state, invalidate file descriptors */
  memset(state, 0, sizeof(*state));

  /* initialize UI */
  ui_state_init(&state->ui);

  /* TODO any additional client state initialization */
  setvbuf(stdout, NULL, _IONBF, 0);

  return 0;
}

static void client_state_free(struct client_state *state)
{

  /* TODO any additional client state cleanup */

  /* cleanup API state */
  api_state_free(&state->api);

  /* cleanup UI state */
  ui_state_free(&state->ui);
}

static void usage(void)
{
  printf("usage:\n");
  printf("  client host port\n");
  exit(1);
}

int main(int argc, char **argv)
{
  int fd;
  uint16_t port;
  struct client_state state;

  /* check arguments */
  if (argc != 3)
    usage();
  if (parse_port(argv[2], &port) != 0)
    usage();

  /* preparations */
  client_state_init(&state);

  /* connect to server */
  fd = client_connect(&state, argv[1], port);
  if (fd < 0)
    return 1;

  /* initialize API */
  api_state_init(&state.api, fd);

  /* TODO any additional client initialization */

  /* client things */
  while (!state.eof && handle_incoming(&state) == 0)
    ;

  /* clean up */
  /* TODO any additional client cleanup */
  client_state_free(&state);
  close(fd);

  return 0;
}
