#define _CRT_SECURE_NO_WARNINGS

#include "logs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char** logs = NULL;

//safe strncat as strcat_s is windows only
void strncat_c(char* dest, const char* src, size_t dest_size) {
    strncat(dest, src, dest_size - strlen(dest) - 1);
}

void create_log(const char* message, ...) {
    if (!logs) {
        logs = calloc(MAX_LOGS, sizeof(char*));
        if (!logs) return;
    }

    char buf[256];

    va_list args;
    va_start(args, message);
    vsnprintf(buf, sizeof(buf), message, args);
    va_end(args);

    if (logs[MAX_LOGS - 1]) {
        free(logs[MAX_LOGS - 1]);
    }

    for (int i = MAX_LOGS - 1; i > 0; i--) {
        logs[i] = logs[i - 1];
    }

    //microsoft is annoying.
#ifdef _WIN32
    logs[0] = _strdup(buf);
#else
    logs[0] = strdup(buf);
#endif
}

void clear_logs() {
    if (!logs) return;

    for (int i = 0; i < MAX_LOGS; i++) {
        if (logs[i]) {
            free(logs[i]);
            logs[i] = NULL;
        }
    }

    free(logs);
    logs = NULL;
}

char* get_log_buffer() {
    if (!logs) return NULL;

    size_t total_len = 0;

    for (int i = 0; i < MAX_LOGS; i++) {
        if (logs[i]) total_len += strlen(logs[i]) + 1;
    }
    if (total_len == 0) return NULL;
    total_len++; //for '\0'

    char* buffer = malloc(total_len);
    if (!buffer) return NULL;

    buffer[0] = '\0';

    for (int i = 0; i < MAX_LOGS; i++) {
        if (!logs[i]) continue;

        strncat_c(buffer, logs[i], total_len);
        strncat_c(buffer, "\n", total_len);
    }

    return buffer;
}
