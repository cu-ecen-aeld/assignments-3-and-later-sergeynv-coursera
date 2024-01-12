#include "common.h"
#include "fileutils.h"
#include "logging.h"
#include "signals.h"
#include <arpa/inet.h>      // inet_ntoa()
#include <errno.h>
#include <netdb.h>          // struct addrinfo
#include <stdio.h>          // printf()
#include <stdlib.h>         // EXIT_FAILURE
#include <string.h>         // strerror(), strcmp()
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>       // umask()
#include <unistd.h>         // close()
#include <inttypes.h>
#include <stdbool.h>

#define PORT_STR "9000"
#define	SOCKET_FAILURE -1

int open_socket(const char * port_str);

void handle_connection(const struct sockaddr_in *, int fd);

static bool d; // run as daemon.
static FILE * tmp_file;

int sock_fd = -1;
int conn_fd = -1;

void parse_args(int, char**);
void daemonize();

int main(int argc, char** argv) {
    parse_args(argc, argv);

    log_i("aesdsocket (server) starting...\n");

    // First of all, install signal SIGINT and SIGTERM handlers.
    // We can do it here, before possibly "daemonizing", because
    // a child process inherits signal actions of the parent on fork.
    install_sigint_sigterm_handlers();

    // https://linux.die.net/man/3/fdopen
    // "a+": 
    // Open for reading and appending (writing at end of file). 
    // The file is created if it does not exist. 
    // The initial file position for reading is at the beginning of the file,
    // but output is always appended to the end of the file.
    tmp_file = fopen(TMP_FILE_PATH, "w+");
    if (!tmp_file) {
        log_e("fopen() failed (%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    #ifdef DEBUG
    log_i("  " TMP_FILE_PATH " opened.\n");
    #endif

    // Try to open (bind()) socket.
    sock_fd = open_socket(PORT_STR);
    
    // If we need to start (fork()) a daemon, now - after we bound our desired port -
    // is the time.
    if (d) daemonize();

    // Start "listening" (mark the socket as accepting connections).
    // https://pubs.opengroup.org/onlinepubs/009695399/functions/listen.html
    if (listen(sock_fd, /* backlog */ 0) != 0) {
        log_e("listen() failed (%d): %s\n", errno, strerror(errno));
        exit(SOCKET_FAILURE);
    }

    // Wait for and accept connections.
    // https://man7.org/linux/man-pages/man2/accept.2.html
    struct sockaddr_in addr_in;
    struct sockaddr * addr_ptr = (struct sockaddr *) &addr_in;
    socklen_t addrlen;
    do {
        log_i("\nWaiting for connections...\n");

        conn_fd = accept(sock_fd, addr_ptr, &addrlen);
        if (conn_fd == -1) {
            log_e("accept() failed (%d): %s\n", errno, strerror(errno));
            exit(SOCKET_FAILURE);
        }
         
        handle_connection(&addr_in, conn_fd);
        conn_fd = -1;
    } while (1);
}

int open_socket(const char * port_str) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;                       // For IPv4.
    hints.ai_socktype = SOCK_STREAM;                 // Stream (TCP) socket.
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
    hints.ai_protocol = 0;                           // Any (suitable) protocol.
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    /*
    https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

    int getaddrinfo(const char *restrict node,
                const char *restrict service,
                const struct addrinfo *restrict hints,
                struct addrinfo **restrict res);

    Either node or service, but not both, may be NULL.

    service sets the port in each returned address structure.

    If AI_NUMERICSERV is specified in hints.ai_flags and service is not
    NULL, then service must point to a string containing a numeric port number.
    */
    struct addrinfo * result;
    int rc = getaddrinfo(NULL, port_str, &hints, &result);
    if (rc != 0) {
        log_e("getaddrinfo() failed (): %s\n", rc, gai_strerror(rc));
        exit(SOCKET_FAILURE);
    }

    struct addrinfo * rp;
    int fd;
    size_t nattempts = 0;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        #ifdef DEBUG
        log_i("  socket() / bind() attempt #%zu\n", ++nattempts);
        {
            const struct sockaddr_in * addr_in = (struct sockaddr_in *) rp;
            const char * in_addr_str = inet_ntoa(addr_in->sin_addr);
            printf("    addr=%s\n", in_addr_str);
        }
        #endif // #ifdef DEBUG

        // First: get an FD.
        // family, socktype and protocal match values in hints.
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            log_e("socket(AF_INET, SOCK_STREAM, 0) failed (%d): %s\n", errno, strerror(errno));
            continue;
        }
        #ifdef DEBUG
        log_i("    fd=%d\n", fd);
        #endif // #ifdef DEBUG

        // Second: bind to address.
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }

        //  If bind() failed: close the FD, and try again (if there are more addrinfo-s).
        close(fd);
    }

    // No longer needed.
    freeaddrinfo(result);

    // No address "succeeded".
    if (rp == NULL) {               
        log_e("Could not bind() (%d attempt(s))\n", nattempts);
        exit(SOCKET_FAILURE);
    }

    return fd;
}

void handle_connection(const struct sockaddr_in * addr_in, int conn_fd) {
    char * in_addr_str = inet_ntoa(addr_in->sin_addr);
    log_i("Accepted connection from %s\n", in_addr_str);

    #ifdef DEBUG
    log_i("  conn_fd=%d", conn_fd);
    log_i("  addr_in=%s:%" PRIu16 "\n", in_addr_str, addr_in->sin_port);
    #endif
    
    // https://man7.org/linux/man-pages/man2/recv.2.html
    // "Convert" to FILE, so that we can use getline() to read line-by-line.
    FILE * sock_file = fdopen(conn_fd, "r+");

    while (copy_next_line(sock_file, tmp_file)) {
        copy_all(tmp_file, sock_file);
    }

    fclose(sock_file);

    // IMPORTANT: do NOT re-use the value stored `in_addr_str` pointer.
    // Re-using the pointer after using it one in log_i() above causes segfaults
    // when running "under" valgrind.
    in_addr_str = inet_ntoa(addr_in->sin_addr);
    log_i("Closed connection from %s\n", in_addr_str);
}

void daemonize() {
    log_i("aesdsocket \"daemonizing\"...\n");
    const pid_t pid = fork();

    if (pid == -1) {
        log_e("fork() failed (%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child / daemon process.
        log_i("aesdsocket daemon running...\n");

        // Reset file umask.
        umask(0);

        if (setsid() == -1) {
            log_e("setsid() failed (%d): %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Change working directory to root.
        if (chdir("/") < 0) {
            log_e("chdir(\"/\") failed (%d): %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Redirect stdin to /dev/null.
        stdin  = freopen("/dev/null", "r", stdin);
        // Redirect stdout, and stderr to /dev/null, unless debugging.
        #ifndef DEBUG
        stdout = freopen("/dev/null", "a", stdout);
        stderr = freopen("/dev/null", "a", stderr);
        #endif
    } else {
        // "Parent" process.
        log_i("aesdsocket daemon started (pid=%ld), exiting...\n", pid);

        // "Close" FDs for the socket and the tmp file explicitely,
        // although it's probably unnecessary.
        fclose(tmp_file);
        close(sock_fd);

        exit(EXIT_SUCCESS);
    }
}

void parse_args(int argc, char** argv) {
    if (argc == 1) {
        d = false;
    } else if (argc == 2 && strcmp("-d", argv[1]) == 0) {
        d = true;
    } else {
        printf("invalid options:");
        for (int i = 0; i < argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\nusage: aesdsocket [-d]\n");
        exit(EXIT_FAILURE);
    }
}
