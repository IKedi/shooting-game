#ifdef _WIN32
#include <conio.h>
#include <stdlib.h>
//literally just a conio.h wrapper

void clear_screen() { //linux one also works
    system("cls"); //however it causes flickering on my device so this seems to work better
}

void init_keyboard() {} //lulz
int kbhit_c() { return _kbhit(); }
int getch_c() { return _getch(); }

//not quite sure why anyone would even want to play this on their linux machine
//but im all for a better linux
//(please just make linux work so i can switch to it without getting rid of billion things i use already)
#else
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void init_keyboard() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int kbhit_c() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

int getch_c() {
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) < 0) return 0;
    return c;
}
#endif
