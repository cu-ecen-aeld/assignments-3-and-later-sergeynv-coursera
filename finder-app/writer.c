#include <errno.h>
#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vprintf(), stderr
#include <stdlib.h> // EXIT_FAILURE/_SUCCESS
#include <string.h> // strerror
#include <syslog.h>

void log_i(const char*, ...);
void log_e(const char*, ...);

int main(int argc, char** argv) {
    openlog(NULL, 0, LOG_USER);

    if (argc != 3) {
        log_e("Wrong number of arguments: %d\n", argc - 1);
        printf("Usage: writer <file> <content>\n");
        return EXIT_FAILURE;
    }

    const char* fname = argv[1];
    const char* content = argv[2];

    log_i("Writing \"%s\" to %s\n", content, fname);

    // Open file with "w" (creates a new file if doesn't already exist, 
    // otherwise destroys existing content).
    FILE* fp = fopen(fname, "w");
    if (!fp) {
        const int fopen_errno = errno;
        log_e("fopen() failed (%d): %s\n", fopen_errno, strerror(fopen_errno));
        return fopen_errno;
    }

    // Write to file.
    if (fputs(content, fp) < 0) {
        const int fputs_errno = errno;
        log_e("fputs() failed (%d): %s\n", fputs_errno, strerror(fputs_errno));
        return fputs_errno;
    }

    // Add line break (we'll ignore errors here).
    fputc('\n', fp);    
    fclose(fp);

    return EXIT_SUCCESS;
}

void vlog(FILE* out, int logpri, const char* format, va_list pa) {
    // Use "v-version" of both syslog, and printf,
    // which take va_list instead of `...`.
    // IMPORTANT: call syslog() first!
    vsyslog(logpri, format, pa);
    vfprintf(out, format, pa);
}

void log_i(const char* format, ...) {
    va_list pa;
    va_start(pa, format);
    // On Debian LOG_DEBUG does appear in /var/log/syslog (but LOG_INFO does).
    vlog(stdout, LOG_DEBUG, format, pa);
    va_end(pa);
}

void log_e(const char* format, ...) {
    va_list pa;
    va_start(pa, format);
    vlog(stderr, LOG_ERR, format, pa);
    va_end(pa);
}
