#include "logging.h"
#include <stdarg.h>  // va_list, va_start, va_end
#include <stdio.h>   // vfprintf, stdout, stderr

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
