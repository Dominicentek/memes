#ifndef ANSI_H
#define ANSI_H

#define MOVE_ABSOLUTE(y, x) "\e[" #y ";" #x "H"
#define MOVE_UP(x) "\e[" #x "A"
#define MOVE_LEFT(x) "\e[" #x "D"
#define MOVE_DOWN(x) "\e[" #x "B"
#define MOVE_RIGHT(x) "\e[" #x "C"

#define MOVE_ABSOLUTE_FMT "\e[%d;%dH"
#define MOVE_UP_FMT "\e[%dA"
#define MOVE_LEFT_FMT "\e[%dD"
#define MOVE_DOWN_FMT "\e[%dB"
#define MOVE_RIGHT_FMT "\e[%dC"

#define CLEAR_CURSOR_TO_END "\e[0J"
#define CLEAR_START_TO_CURSOR "\e[1J"
#define CLEAR_SCREEN "\e[2J"
#define CLEAR_CURSOR_TO_LINE_END "\e[0K"
#define CLEAR_LINE_START_TO_CURSOR "\e[1K"
#define CLEAR_LINE "\e[2K"

#define ENABLE_BLINK "\e[5m"
#define DISABLE_BLINK "\e[25m"
#define ENABLE_BOLD "\e[1m"
#define DISABLE_BOLD "\e[22m"
#define ENABLE_ITALIC "\e[3m"
#define DISABLE_ITALIC "\e[23m"
#define ENABLE_UNDERLINE "\e[4m"
#define DISABLE_UNDERLINE "\e[24m"
#define ENABLE_STRIKETHROUGH "\e[9m"
#define DISABLE_STRIKETHROUGH "\e[29m"
#define RESET "\e[0m"

#define SHOW_TEXT "\e[28m"
#define HIDE_TEXT "\e[8m"
#define SHOW_CURSOR "\e[?25h"
#define HIDE_CURSOR "\e[?25l"

#define FG_COLOR_DEFAULT "\e[39m"
#define FG_COLOR_256(x) "\e[38;5;" #x "m"
#define FG_COLOR_RGB(r, g, b) "\e[38;2;" #r ";" #g ";" #b "m"
#define BG_COLOR_DEFAULT "\e[49m"
#define BG_COLOR_256(x) "\e[48;5;" #x "m"
#define BG_COLOR_RGB(r, g, b) "\e[48;2;" #r ";" #g ";" #b "m"

#define FG_COLOR_256_FMT "\e[38;5;%dm"
#define FG_COLOR_RGB_FMT "\e[38;2;%d;%d;%dm"
#define BG_COLOR_256_FMT "\e[38;5;%dm"
#define BG_COLOR_RGB_FMT "\e[38;2;%d;%d;%dm"

#define FG_COLOR_BLACK "\e[30m"
#define FG_COLOR_DARK_RED "\e[31m"
#define FG_COLOR_DARK_GREEN "\e[32m"
#define FG_COLOR_DARK_YELLOW "\e[33m"
#define FG_COLOR_DARK_BLUE "\e[34m"
#define FG_COLOR_DARK_MAGENTA "\e[35m"
#define FG_COLOR_DARK_CYAN "\e[36m"
#define FG_COLOR_GRAY "\e[37m"
#define FG_COLOR_DARK_GRAY "\e[82m"
#define FG_COLOR_RED "\e[83m"
#define FG_COLOR_GREEN "\e[84m"
#define FG_COLOR_YELLOW "\e[85m"
#define FG_COLOR_BLUE "\e[86m"
#define FG_COLOR_MAGENTA "\e[87m"
#define FG_COLOR_CYAN "\e[88m"
#define FG_COLOR_WHITE "\e[89m"

#define BG_COLOR_BLACK "\e[40m"
#define BG_COLOR_DARK_RED "\e[41m"
#define BG_COLOR_DARK_GREEN "\e[42m"
#define BG_COLOR_DARK_YELLOW "\e[43m"
#define BG_COLOR_DARK_BLUE "\e[44m"
#define BG_COLOR_DARK_MAGENTA "\e[45m"
#define BG_COLOR_DARK_CYAN "\e[46m"
#define BG_COLOR_GRAY "\e[47m"
#define BG_COLOR_DARK_GRAY "\e[92m"
#define BG_COLOR_RED "\e[93m"
#define BG_COLOR_GREEN "\e[94m"
#define BG_COLOR_YELLOW "\e[95m"
#define BG_COLOR_BLUE "\e[96m"
#define BG_COLOR_MAGENTA "\e[97m"
#define BG_COLOR_CYAN "\e[98m"
#define BG_COLOR_WHITE "\e[99m"

#define BELL "\7"

#endif