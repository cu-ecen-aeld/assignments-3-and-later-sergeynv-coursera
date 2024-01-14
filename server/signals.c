#include "logging.h"
#include "signals.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>          // remove()
#include <stdlib.h>         // EXIT_FAILURE
#include <string.h>         // strerror(), strsignal()
#include <unistd.h>         // _exit()

void _handle(int signo) {
    log_i("\nCaught signal, exiting\n");

    int exit_status = EXIT_SUCCESS;
    #ifdef DEBUG
    log_i("  %s\n", strsignal(signo));
    log_i("  sock_fd=%d\n", sock_fd);
    log_i("  conn_fd=%d\n", conn_fd);
    #endif
    
    if (conn_fd != -1) {
        if (close(conn_fd) != 0) {
            log_e("Error closing connection: %s (%d)", strerror(errno), errno);
            exit_status = EXIT_FAILURE;
        }
        #ifdef DEBUG
        log_i("  conn_fd closed.\n");
        #endif
    }

    if (sock_fd != -1) {
        if (close(sock_fd) != 0) {
            log_e("Error closing socket: %s (%d)", strerror(errno), errno);
            exit_status = EXIT_FAILURE;
        }
        #ifdef DEBUG
        log_i("  sock_fd closed.\n");
        #endif
    }

    if (remove(TMP_FILE_PATH) != 0) {
        log_e("remove(" TMP_FILE_PATH ") failed: %s (%d)", strerror(errno), errno);
        exit_status = EXIT_FAILURE;
    }
    #ifdef DEBUG
    else {
        log_i("  " TMP_FILE_PATH " removed.\n");
    }
    #endif

    if (daemonized) {
        if (remove(DAEMON_PID_FILE_PATH) != 0) {
            log_e("remove(" DAEMON_PID_FILE_PATH ") failed: %s (%d)", strerror(errno), errno);
            // Do NOT set EXIT_FAILURE because of this.
        }
        #ifdef DEBUG
        else {
            log_i("  " DAEMON_PID_FILE_PATH " removed.\n");
        }
        #endif
    }

    _exit(exit_status);
}

void _install_signal_handler(int signo, struct sigaction * handler) {
    // https://man7.org/linux/man-pages/man2/sigaction.2.html
    if(sigaction(signo, handler, NULL) != 0) {
        log_e("Error registering for %s: %s (%d)",
              strsignal(signo), strerror(errno), errno);
        exit(EXIT_FAILURE);
    }    
}

void install_sigint_sigterm_handlers() {
    struct sigaction handler;
    memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_handler = _handle;

    _install_signal_handler(SIGTERM, &handler);
    _install_signal_handler(SIGINT, &handler);
}