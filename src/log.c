// Created by ABai on 2026/3/26.

#include "log.h"
#include <stdio.h>
#include <stdarg.h>
void HC_debug_output(const char *format, ...) {
    if (!HC_DEBUG_MODE) { return; }
    printf("[DEBUG] ");
    va_list ap;
    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);
}