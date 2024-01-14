#include "common.h"
#include <stdbool.h>

// Global variables defined in aesdsocket.c
extern int sock_fd;
extern int conn_fd;
extern bool daemonized;

void install_sigint_sigterm_handlers();
