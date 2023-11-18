#ifndef MY_COLOR_H
#define MY_COLOR_H

#include <stdlib.h>

#define ANSI_RESET "\x1b[0m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_BLUE "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN "\x1b[36m"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>

void activate_color() {
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwmode = 0;
    if (!GetConsoleMode(hout, &dwmode)) {
        printf("GETTING CONSOLE MODE ERROR!\n");
        exit(1);
    }
    dwmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hout, dwmode)) {
        printf("ERROR SETTING CONSOLE MODE!\n");
        exit(1);
    }
}

#else

void activate_color(){}

#endif

#endif