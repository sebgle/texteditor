/*** includes ***/

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void fatalError(const char *s) {
    perror(s);
    exit(1);
}

void rawModeOff() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) fatalError("tcsetattr");
}

void rawModeOn() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) fatalError("tcgetattr");    
    atexit(rawModeOff);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;  

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) fatalError("tcsetattr");
}

/*** init ***/

int main() {
    rawModeOn();

    while(1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) fatalError("read");
        if (iscntrl(c)) { 
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }

    return 0;
}