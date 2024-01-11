#include "fileutils.h"
#include "logging.h"
#include <errno.h>
#include <stdlib.h>     // free()
#include <string.h>     // strerror()

#ifdef DEBUG
#include <stdio.h>      // fileno()
#endif

int copy_next_line(FILE * from, FILE * to) {
    #ifdef DEBUG
    log_i("  copy_next_line from FD #%d to FD #%d\n", fileno(from), fileno(to));
    #endif

    char * line = NULL;
    size_t len = 0;

    ssize_t read = getline(&line, &len, from);
    if (read == -1) return 0;

    #ifdef DEBUG
    printf("    line (len=%zu): %s", read, line);
    #endif

    if (fputs(line, to) < 0) {
        log_e("fputs() failed (%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    free(line);
    return 1;
}

void copy_all(FILE * from, FILE * to) {
    #ifdef DEBUG
    log_i("  copy_all from FD #%d to FD #%d\n", fileno(from), fileno(to));
    #endif

    // Rewind to the beginning of the "source" file.
    rewind(from);

    char * line = NULL;
    size_t len = 0;
    size_t index = 0;
    ssize_t read;
    while ((read = getline(&line, &len, from)) != -1) {
        #ifdef DEBUG
        printf("    line#%zu (len=%zu): %s", ++index, read, line);
        #endif

        if (fputs(line, to) < 0) {
            log_e("fputs() failed (%d): %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    free(line);
}
