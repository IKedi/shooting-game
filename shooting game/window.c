#ifdef _WIN32
#include <conio.h>
#include <stdio.h>
#include <windows.h>
//literally just a conio.h wrapper

void sleep_ms(int ms) {
    Sleep(ms);
}

void set_window_title(const char* title) {
    SetConsoleTitleA(title);
}

void clear_screen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void init_keyboard() {}
void reset_keyboard() {}

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

void sleep_ms(int ms) {
    usleep(ms * 1000);
}

void set_window_title(const char* title) {
    printf("\033]0;%s\007", title);
    fflush(stdout);
}

void clear_screen() {
    printf("\033[2J\033[H");
    fflush(stdout);
}
static struct termios old_t; //keeping it here so its apparent what its used for.

void init_keyboard() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &old_t);
    t = old_t;
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void reset_keyboard() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
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
