#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <thread>
#include <chrono>

#include "ansi.h"
#include "settings.h"
#include "kbhit.h"

#define MAX_MEME_LIMIT 16
#define BEGINNING_ITEMS 5
#define MEME_SEARCH_MAX 256
#define FLUSH fflush(nullptr)
#define END return end()

int optionpos[] = { 3, 4, 5, 6, 8, 11 };

std::filesystem::path meme_folder = std::filesystem::path(MEME_FOLDER_SRC);
std::vector<std::filesystem::path> all_memes = {};
int meme_width = 0;
int meme_search_len = 0;
char meme_search[MEME_SEARCH_MAX];

int selected_item = 0;
int selected_meme = 0;
int selected_meme_opt = 0;

bool meme_playing = false;

void play_meme(int id) {
    meme_playing = true;
    std::thread playing_thread = std::thread([]() {
        std::string spin = "|/-\\";
        int spinptr = 0;
        while (meme_playing) {
            printf(MOVE_ABSOLUTE_FMT FG_COLOR_YELLOW BG_COLOR_DEFAULT "%c" FG_COLOR_DEFAULT " Playing", optionpos[BEGINNING_ITEMS] - 2, 13 + meme_width - 24, spin[spinptr++]);
            FLUSH;
            std::this_thread::sleep_for(std::chrono::milliseconds(125));
            if (spinptr == spin.length()) spinptr = 0;
        }
    });
    system(("ffplay -autoexit -hide_banner -loglevel error \"" + all_memes[id].string() + "\"").c_str());
    meme_playing = false;
    playing_thread.join();
    printf(MOVE_ABSOLUTE_FMT "         ", optionpos[BEGINNING_ITEMS] - 2, 13 + meme_width - 24);
}

void refresh_directory() {
    all_memes.clear();
    std::string search_term = std::string(meme_search);
    std::transform(search_term.begin(), search_term.end(), search_term.begin(), ::tolower);
    for (auto& entry : std::filesystem::recursive_directory_iterator(meme_folder)) {
        if (std::filesystem::is_directory(entry.path())) continue;
        if (meme_width < entry.path().filename().string().length()) meme_width = entry.path().filename().string().length();
        std::string filename = entry.path().filename();
        std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
        if (filename.find(search_term) == std::string::npos) continue;
        all_memes.push_back(entry.path());
    }
}

void print_meme_table() {
    int offset = selected_item - BEGINNING_ITEMS - MAX_MEME_LIMIT / 2;
    offset = std::min(std::max(offset, 0), std::max((int)all_memes.size() - MAX_MEME_LIMIT, 0));
    printf(MOVE_ABSOLUTE_FMT BG_COLOR_DEFAULT FG_COLOR_DEFAULT CLEAR_LINE FG_COLOR_YELLOW "%d" FG_COLOR_DEFAULT " memes found", optionpos[BEGINNING_ITEMS] - 1, 7, all_memes.size());
    for (int i = 0; i < MAX_MEME_LIMIT; i++) {
        if (i >= all_memes.size()) {
            printf(MOVE_ABSOLUTE_FMT BG_COLOR_DEFAULT FG_COLOR_DEFAULT CLEAR_LINE, optionpos[BEGINNING_ITEMS] + i, 7);
            continue;
        }
        std::string filename = all_memes[offset + i].filename().string();
        printf(MOVE_ABSOLUTE_FMT BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ %s%s ] ", optionpos[BEGINNING_ITEMS] + i, 7, filename.c_str(), std::string(meme_width - filename.length(), ' ').c_str());
    }
}

void print_cursor(char cursor) {
    int pos = 0;
    int offset = selected_item - BEGINNING_ITEMS - MAX_MEME_LIMIT / 2;
    offset = std::min(std::max(offset, 0), std::max((int)all_memes.size() - MAX_MEME_LIMIT, 0));
    if (selected_item < BEGINNING_ITEMS) pos = optionpos[selected_item];
    else pos = optionpos[BEGINNING_ITEMS] + (selected_item - BEGINNING_ITEMS) - offset;
    printf(BG_COLOR_DEFAULT FG_COLOR_DEFAULT MOVE_ABSOLUTE_FMT "%c", pos, 5, cursor);
}

void print_meme_action(bool clear) {
    int x = 13 + meme_width - 24;
    int y = optionpos[BEGINNING_ITEMS] - 1;
    if (clear) {
        printf(MOVE_ABSOLUTE_FMT BG_COLOR_DEFAULT FG_COLOR_DEFAULT "                        ", y, x);
        return;
    }
    printf(MOVE_ABSOLUTE_FMT BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ CANCEL  PLAY  SEND ] ", y, x);
    if (selected_meme_opt == 0) printf(MOVE_ABSOLUTE_FMT BG_COLOR_WHITE FG_COLOR_DARK_BLUE " CANCEL ", y, x + 2);
    if (selected_meme_opt == 1) printf(MOVE_ABSOLUTE_FMT BG_COLOR_WHITE FG_COLOR_DARK_BLUE " PLAY ",   y, x + 10);
    if (selected_meme_opt == 2) printf(MOVE_ABSOLUTE_FMT BG_COLOR_WHITE FG_COLOR_DARK_BLUE " SEND ",   y, x + 16);
}

int end() {
    printf(MOVE_ABSOLUTE(0, 0) BG_COLOR_DEFAULT FG_COLOR_DEFAULT CLEAR_SCREEN);
    printf(SHOW_CURSOR);
    return 0;
}

void print_search() {
    int margin = std::max(0, 16 - meme_search_len);
    printf(MOVE_ABSOLUTE_FMT BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ 🔎 %s%s ] ", optionpos[4], 7, meme_search + std::max(0, meme_search_len - 16), std::string(margin, ' ').c_str());
}

int main() {
    refresh_directory();
    printf(CLEAR_SCREEN);
    printf(MOVE_ABSOLUTE_FMT "> " BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Play All    ] ",         optionpos[0], 5);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Send Random ] ",         optionpos[1], 7);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Download    ] ",         optionpos[2], 7);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Refresh     ] ",         optionpos[3], 7);
    print_search();
    printf(MOVE_ABSOLUTE(3, 37) BG_COLOR_DEFAULT FG_COLOR_DEFAULT "Use the arrow keys to navigate.");
    printf(MOVE_ABSOLUTE(4, 37) BG_COLOR_DEFAULT FG_COLOR_DEFAULT "Enter to select.");
    printf(MOVE_ABSOLUTE(5, 37) BG_COLOR_DEFAULT FG_COLOR_DEFAULT "Ctrl+C or ESC to quit.");
    print_meme_table();
    printf(HIDE_CURSOR);
    FLUSH;
    keyboard kb;
    while (true) {
        if (kb.kbhit()) {
            int kc = kb.getch();
            while (kb.kbhit()) kc = kc * 256 + kb.getch();
            if (kc == 3 || kc == 27) END;
            print_cursor(' ');
            if (kc == 1792833) selected_item--;
            if (kc == 1792834) selected_item++;
            if (selected_item < 0) selected_item = 0;
            if (selected_item > BEGINNING_ITEMS + all_memes.size() - 1) selected_item = all_memes.size() - 1 + BEGINNING_ITEMS;
            print_cursor('>');
            if (kc == 10) {
                if (selected_item == 3) refresh_directory();
                if (selected_item >= BEGINNING_ITEMS) {
                    selected_meme = selected_item - BEGINNING_ITEMS;
                    selected_meme_opt = 2;
                    print_meme_action(false);
                    FLUSH;
                    while (true) {
                        if (!kb.kbhit()) continue;
                        kc = kb.getch();
                        while (kb.kbhit()) kc = kc * 256 + kb.getch();
                        if (kc == 3) END;
                        if (kc == 27) break;
                        if (kc == 1792836) selected_meme_opt--;
                        if (kc == 1792835) selected_meme_opt++;
                        if (selected_meme_opt < 0) selected_meme_opt = 0;
                        if (selected_meme_opt > 2) selected_meme_opt = 2;
                        if (kc == 10) {
                            if (selected_meme_opt == 1) play_meme(selected_meme);
                            break;
                        }
                        print_meme_action(false);
                        FLUSH;
                    }
                    print_meme_action(true);
                }
            }
            if (selected_item == 4 && (kc >= 32 && kc <= 127)) {
                if (kc == 127) { if (meme_search_len > 0) meme_search_len--; }
                else if (meme_search_len != MEME_SEARCH_MAX - 1) meme_search[meme_search_len++] = kc;
                meme_search[meme_search_len] = 0;
                print_search();
                refresh_directory();
            }
            print_meme_table();
            if (selected_item == 4) printf(MOVE_ABSOLUTE_FMT SHOW_CURSOR, optionpos[4], 13 + std::min(16, meme_search_len));
            else printf(HIDE_CURSOR);
            FLUSH;
        }
    }
    END;
}