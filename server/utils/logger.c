#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

static FILE *log_file = NULL;

void logger_init(const char *filename) {
    log_file = fopen(filename, "w");
}

void logger_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(log_file ? log_file : stdout, format, args);
    fprintf(log_file ? log_file : stdout, "\n");
    va_end(args);
    if (log_file) fflush(log_file);
}

void logger_destroy() {
    if (log_file) fclose(log_file);
}