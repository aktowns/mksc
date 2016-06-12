#ifndef __ANSI_COLOUR_H
#define __ANSI_COLOUR_H

#define RGB(r, g, b) "\e[38;2;" #r ";" #g ";" #b "m"

#define ANSI_NORMAL "\x1B[0m"
#define ANSI_RED "\x1B[31m"
#define ANSI_GREEN "\x1B[32m"
#define ANSI_YELLOW "\x1B[33m"
#define ANSI_BLUE "\x1B[34m"
#define ANSI_MAGENTA "\x1B[35m"
#define ANSI_CYAN "\x1B[36m"
#define ANSI_WHITE "\x1B[37m"

#endif
