#pragma once
//crossplatform conio.h thing and console helper

void sleep_ms(int ms); //changed name to not confuse with POSIX

void set_window_title(const char* title);
void clear_screen();

void init_keyboard();
void reset_keyboard();

int kbhit_c();
int getch_c();