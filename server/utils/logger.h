#ifndef LOGGER_H
#define LOGGER_H

void logger_init(const char *filename);
void logger_log(const char *format, ...);
void logger_destroy();

#endif