#pragma once

#define MAX_LOGS 10
char** logs;

void create_log(const char* message, ...);
void clear_logs();
char* get_log_buffer();