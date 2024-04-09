#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <fstream>

#include "ansi.h"
#include "kbhit.h"

#define CHANNEL(name, id) { name, id },

std::vector<std::pair<std::string, std::string>> channels = {
#include "settings.h"
};

#define MAX_MEME_LIMIT 16
#define MAX_CHANNEL_LIMIT 5
#define BEGINNING_ITEMS 6
#define MEME_SEARCH_MAX 256
#define FLUSH fflush(nullptr)
#define END return end()
#define SLEEP(millis) std::this_thread::sleep_for(std::chrono::milliseconds(millis))

int optionpos[] = { 3, 4, 5, 6, 7, 9, 12 };

std::filesystem::path meme_folder = std::filesystem::path(MEME_FOLDER_SRC);
std::vector<std::filesystem::path> all_memes = {};
int meme_width = 0;
int meme_search_len = 0;
char meme_search[MEME_SEARCH_MAX];

int selected_item = 0;
int selected_meme = 0;
int selected_meme_opt = 0;

bool meme_playing = false;
bool meme_stop_playing = false;
int meme_counter = 0;

std::string curr_channel_name = channels[0].first;
std::string curr_channel_id = channels[0].second;
int curr_channel_index = 0;

void play_meme(int id, int y, int x) {
    meme_playing = true;
    std::thread playing_thread = std::thread([&]() {
        std::string spin = "|/-\\";
        int spinptr = 0;
        while (meme_playing) {
            printf(MOVE_ABSOLUTE_FMT FG_COLOR_YELLOW BG_COLOR_DEFAULT "%c" FG_COLOR_DEFAULT " Playing", y, x, spin[spinptr++]);
            FLUSH;
            SLEEP(125);
            if (spinptr == spin.length()) spinptr = 0;
        }
    });
    system(("ffplay -autoexit -hide_banner -loglevel error \"" + all_memes[id].string() + "\"").c_str());
    meme_playing = false;
    playing_thread.join();
    printf(MOVE_ABSOLUTE_FMT "         ", y, x);
}

void play_all_memes() {
    meme_playing = true;
    meme_stop_playing = false;
    meme_counter = 0;
    std::thread playing_thread = std::thread([]() {
        std::string spin = "|/-\\";
        int spinptr = 0;
        keyboard kb;
        while (meme_playing) {
            if (meme_stop_playing) printf(MOVE_ABSOLUTE_FMT FG_COLOR_YELLOW BG_COLOR_DEFAULT "%c" FG_COLOR_DEFAULT " Waiting for video to finish", optionpos[0], 25, spin[spinptr++]);
            else printf(MOVE_ABSOLUTE_FMT FG_COLOR_YELLOW BG_COLOR_DEFAULT "%c" FG_COLOR_DEFAULT " (%d/%d)", optionpos[0], 25, spin[spinptr++], meme_counter + 1, all_memes.size());
            FLUSH;
            SLEEP(125);
            if (spinptr == spin.length()) spinptr = 0;
            while (kb.kbhit()) {
                if (kb.getch() == 3) meme_stop_playing = true;
            }
        }
    });
    std::vector<std::filesystem::path> shuffled = {};
    for (int i = 0; i < all_memes.size(); i++) {
        shuffled.push_back(all_memes[i]);
    }
    for (int i = 0; i < all_memes.size() && !meme_stop_playing; i++) {
        int idx = rand() % shuffled.size();
        system(("ffplay -autoexit -hide_banner -loglevel error \"" + shuffled[idx].string() + "\"").c_str());
        shuffled.erase(shuffled.begin() + idx);
        meme_counter++;
    }
    meme_playing = false;
    playing_thread.join();
    printf(MOVE_ABSOLUTE_FMT FG_COLOR_DEFAULT BG_COLOR_DEFAULT CLEAR_CURSOR_TO_LINE_END, optionpos[0], 25);
}

#define swrite(s) write(s, sizeof(s) - 1);

void send_meme(int id, int y, int x) {
    std::ofstream out = std::ofstream("/tmp/body.dat", std::ios::binary);
    std::string name = all_memes[id].filename().string();
    std::ifstream in = std::ifstream(all_memes[id], std::ios::binary);
    int datalen = std::filesystem::file_size(all_memes[id]);
    char* data = (char*)malloc(datalen);
    in.read(data, datalen);
    in.close();
    out.swrite("--boundary\n");
    out.swrite("Content-Disposition: form-data; name=\"payload_json\"\n");
    out.swrite("Content-Type: application/json\n");
    out.swrite("\n");
    out.swrite("{\n");
    out.swrite("  \"attachments\": [{\n");
    out.swrite("    \"id\": 0,\n");
    out.swrite("    \"filename\": \"");
    out.write(name.c_str(), name.length()); 
    out.swrite("\"\n");
    out.swrite("  }]\n");
    out.swrite("}\n");
    out.swrite("--boundary\n");
    out.swrite("Content-Disposition: form-data; name=\"tts\"\n");
    out.swrite("\n");
    out.swrite("false\n");
    out.swrite("--boundary\n");
    out.swrite("Content-Disposition: form-data; name=\"files[0]\"; filename=\"");
    out.write(name.c_str(), name.length());
    out.swrite("\"\n");
    out.swrite("Content-Type: video/mp4\n");
    out.swrite("\n");
    out.write(data, datalen);
    out.swrite("\n--boundary--\n");
    out.close();
    free(data);
    bool sending = true;
    std::thread sending_thread = std::thread([&]() {
        std::string spin = "|/-\\";
        int spinptr = 0;
        while (sending) {
            printf(MOVE_ABSOLUTE_FMT FG_COLOR_YELLOW BG_COLOR_DEFAULT "%c" FG_COLOR_DEFAULT " Sending", y, x, spin[spinptr++]);
            if (spinptr == spin.length()) spinptr = 0;
            FLUSH;
            SLEEP(125);
        }
    });
    system(("curl -X POST --header \"Content-Type: multipart/form-data; boundary=boundary\" --header \"Accept: */*\" --header \"Authorization: " DISCORD_TOKEN "\" --data-binary @/tmp/body.dat https://discord.com/api/v9/channels/" + curr_channel_id + "/messages 1> /dev/null 2> /dev/null").c_str());
    sending = false;
    sending_thread.join();
    printf(MOVE_ABSOLUTE_FMT "         ", y, x);
    FLUSH;
}

int get_random() {
    return rand() % all_memes.size();
}

void print_channel_switcher() {
    int offset = curr_channel_index - MAX_CHANNEL_LIMIT / 2;
    offset = std::min(std::max(offset, 0), std::max((int)channels.size() - MAX_CHANNEL_LIMIT, 0));
    int maxlen = 0;
    for (int i = 0; i < channels.size(); i++) {
        if (maxlen < channels[i].first.length()) maxlen = channels[i].first.length();
    }
    for (int i = 0; i <= MAX_CHANNEL_LIMIT; i++) {
        if (i >= channels.size()) break;
        if (i == curr_channel_index) printf(MOVE_ABSOLUTE_FMT
            BG_COLOR_DEFAULT FG_COLOR_DEFAULT
            BG_COLOR_WHITE FG_COLOR_DARK_BLUE
            " [ %-*s ] ",
            optionpos[3] + i, 34, maxlen, channels[i].first.c_str()
        );
        else printf(MOVE_ABSOLUTE_FMT
            BG_COLOR_DEFAULT FG_COLOR_DEFAULT
            BG_COLOR_DARK_BLUE FG_COLOR_WHITE
            " [ %-*s ] ",
            optionpos[3] + i, 34, maxlen, channels[i].first.c_str()
        );
    }
    FLUSH;
}

void clear_channel_switcher() {
    printf(BG_COLOR_DEFAULT FG_COLOR_DEFAULT);
    for (int i = 0; i < 5; i++) {
        printf(MOVE_ABSOLUTE_FMT CLEAR_CURSOR_TO_LINE_END, optionpos[3] + i, 34);
    }
    FLUSH;
}

void switch_channel() {
    print_channel_switcher();
    keyboard kb;
    int prev = curr_channel_index;
    while (true) {
        if (!kb.kbhit()) {
            SLEEP(50);
            continue;
        }
        int kc = kb.getch();
        while (kb.kbhit()) kc = kc * 256 + kb.getch();
        if (kc == 1792833) curr_channel_index--;
        if (kc == 1792834) curr_channel_index++;
        if (kc == 3) {
            curr_channel_index = prev;
            break;
        }
        if (kc == 10) break;
        if (curr_channel_index < 0) curr_channel_index = 0;
        if (curr_channel_index >= channels.size()) curr_channel_index = channels.size() - 1;
        print_channel_switcher();
        SLEEP(50);
    }
    curr_channel_name = channels[curr_channel_index].first;
    curr_channel_id = channels[curr_channel_index].second;
    clear_channel_switcher();
}

void refresh_directory() {
    all_memes.clear();
    std::string search_term = std::string(meme_search);
    std::transform(search_term.begin(), search_term.end(), search_term.begin(), ::tolower);
    for (auto& entry : std::filesystem::directory_iterator(meme_folder)) {
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
    printf(MOVE_ABSOLUTE_FMT BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ 🔎 %s%s ] ", optionpos[5], 7, meme_search + std::max(0, meme_search_len - 16), std::string(margin, ' ').c_str());
}

int main() {
    srand(clock());
    refresh_directory();
    printf(CLEAR_SCREEN);
    printf(MOVE_ABSOLUTE_FMT "> " BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Play All    ] ",         optionpos[0], 5);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Send Random ] ",         optionpos[1], 7);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Play Random ] ",         optionpos[2], 7);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Channel     ] ",         optionpos[3], 7);
    printf(MOVE_ABSOLUTE_FMT      BG_COLOR_DARK_BLUE FG_COLOR_WHITE " [ Refresh     ] ",         optionpos[4], 7);
    print_search();
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
                if (selected_item == 0) play_all_memes();
                if (selected_item == 1) send_meme(get_random(), optionpos[1], 25);
                if (selected_item == 2) play_meme(get_random(), optionpos[2], 25);
                if (selected_item == 3) switch_channel();
                if (selected_item == 4) refresh_directory();
                if (selected_item >= BEGINNING_ITEMS) {
                    selected_meme = selected_item - BEGINNING_ITEMS;
                    selected_meme_opt = 2;
                    print_meme_action(false);
                    FLUSH;
                    while (true) {
                        if (!kb.kbhit()) {
                            SLEEP(50);
                            continue;
                        }
                        kc = kb.getch();
                        while (kb.kbhit()) kc = kc * 256 + kb.getch();
                        if (kc == 3) END;
                        if (kc == 27) break;
                        if (kc == 1792836) selected_meme_opt--;
                        if (kc == 1792835) selected_meme_opt++;
                        if (selected_meme_opt < 0) selected_meme_opt = 0;
                        if (selected_meme_opt > 2) selected_meme_opt = 2;
                        if (kc == 10) {
                            if (selected_meme_opt == 2) send_meme(selected_meme, optionpos[BEGINNING_ITEMS] - 2, 13 + meme_width - 24);
                            if (selected_meme_opt == 1) play_meme(selected_meme, optionpos[BEGINNING_ITEMS] - 2, 13 + meme_width - 24);
                            break;
                        }
                        print_meme_action(false);
                        FLUSH;
                        SLEEP(50);
                    }
                    print_meme_action(true);
                }
            }
            if (selected_item == 5 && (kc >= 32 && kc <= 127)) {
                if (kc == 127) { if (meme_search_len > 0) meme_search_len--; }
                else if (meme_search_len != MEME_SEARCH_MAX - 1) meme_search[meme_search_len++] = kc;
                meme_search[meme_search_len] = 0;
                print_search();
                refresh_directory();
            }
            print_meme_table();
            if (selected_item == 3) printf(MOVE_ABSOLUTE_FMT BG_COLOR_DEFAULT FG_COLOR_DEFAULT "%s", optionpos[3], 25, curr_channel_name.c_str());
            else printf(MOVE_ABSOLUTE_FMT BG_COLOR_DEFAULT FG_COLOR_DEFAULT CLEAR_CURSOR_TO_LINE_END, optionpos[3], 25);
            if (selected_item == 5) printf(MOVE_ABSOLUTE_FMT SHOW_CURSOR, optionpos[5], 13 + std::min(16, meme_search_len));
            else printf(HIDE_CURSOR);
            FLUSH;
        }
        SLEEP(50);
    }
    END;
}