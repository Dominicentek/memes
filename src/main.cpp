#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include "ansi.h"
#include "settings.h"
#include "kbhit.h"

#define MAX_MEME_LIMIT 16
#define BEGINNING_ITEMS 3
#define FLUSH fflush(nullptr)

int optionpos[] = { 3, 4, 6, 9 };

std::filesystem::path meme_folder = std::filesystem::path(MEME_FOLDER_SRC);
std::vector<std::filesystem::path> all_memes = {};
int meme_width = 0;

int selected_item = 0;

void refresh_directory() {
    all_memes.clear();
    for (auto& entry : std::filesystem::recursive_directory_iterator(meme_folder)) {
        if (std::filesystem::is_directory(entry.path())) continue;
        if (meme_width < entry.path().filename().string().length()) meme_width = entry.path().filename().string().length();
        all_memes.push_back(entry.path());
    }
}

void print_meme_table() {
    int offset = selected_item - BEGINNING_ITEMS - MAX_MEME_LIMIT / 2;
    offset = std::min(std::max(offset, 0), (int)all_memes.size() - MAX_MEME_LIMIT);
    for (int i = 0; i < MAX_MEME_LIMIT; i++) {
        std::string filename = all_memes[offset + i].filename().string().c_str();
        printf(MOVE_ABSOLUTE_FMT BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ %s%s ] ", optionpos[BEGINNING_ITEMS] + i, 7, filename.c_str(), std::string(meme_width - filename.length(), ' ').c_str());
    }
}

void print_cursor(char cursor) {
    int pos = 0;
    int offset = selected_item - BEGINNING_ITEMS - MAX_MEME_LIMIT / 2;
    offset = std::min(std::max(offset, 0), (int)all_memes.size() - MAX_MEME_LIMIT);
    if (selected_item < BEGINNING_ITEMS) pos = optionpos[selected_item];
    else pos = optionpos[BEGINNING_ITEMS] + (selected_item - 3) - offset;
    printf(BG_COLOR_DEFAULT FG_COLOR_DEFAULT MOVE_ABSOLUTE_FMT "%c", pos, 5, cursor);
}

int main() {
    refresh_directory();
    printf(CLEAR_SCREEN);
    printf(MOVE_ABSOLUTE_FMT "> " BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Play All    ] ",         optionpos[0], 5);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Send Random ] ",         optionpos[1], 7);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ 🔎 ________________ ] ", optionpos[2], 7);
    printf(MOVE_ABSOLUTE(3, 37) BG_COLOR_DEFAULT FG_COLOR_DEFAULT "Use the arrow keys");
    printf(MOVE_ABSOLUTE(4, 37) BG_COLOR_DEFAULT FG_COLOR_DEFAULT "to navigate up and down.");
    printf(MOVE_ABSOLUTE(5, 37) BG_COLOR_DEFAULT FG_COLOR_DEFAULT "Enter to select.");
    printf(MOVE_ABSOLUTE_FMT FG_COLOR_YELLOW "%d" FG_COLOR_DEFAULT " memes found", optionpos[BEGINNING_ITEMS] - 1, 7, all_memes.size());
    print_meme_table();
    printf(HIDE_CURSOR);
    FLUSH;
    keyboard kb;
    while (true) {
        if (kb.kbhit()) {
            int kc = kb.getch();
            while (kb.kbhit()) kc = kc * 256 + kb.getch();
            if (kc == 3 || kc == 27) break;
            print_cursor(' ');
            if (kc == 1792833) selected_item--;
            if (kc == 1792834) selected_item++;
            if (selected_item < 0) selected_item = 0;
            if (selected_item > BEGINNING_ITEMS + all_memes.size() - 1) selected_item = all_memes.size() - 1 + BEGINNING_ITEMS;
            print_cursor('>');
            print_meme_table();
            FLUSH;
        }
    }
    printf(MOVE_ABSOLUTE(0, 0) BG_COLOR_DEFAULT FG_COLOR_DEFAULT CLEAR_SCREEN);
    printf(SHOW_CURSOR);
    return 0;
}