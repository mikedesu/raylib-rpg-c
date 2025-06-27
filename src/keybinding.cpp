#include "keybinding.h"
#include "massert.h"
#include "mprint.h"
#include "raylib.h"

using std::shared_ptr;
using std::string;

const char* get_action_for_key(shared_ptr<keybinding_list_t> kb, int key) {
    for (int i = 0; i < kb->count; i++) {
        if (kb->bindings[i].key == key) {
            return kb->bindings[i].action;
        }
    }
    return NULL;
}

//int key_from_string(const char* str) {
int key_from_string(string str) {
    if (str == "ESCAPE") return KEY_ESCAPE;
    if (str == "A") return KEY_A;
    if (str == "B") return KEY_B;
    if (str == "C") return KEY_C;
    if (str == "D") return KEY_D;
    if (str == "E") return KEY_E;
    if (str == "F") return KEY_F;
    if (str == "G") return KEY_G;
    if (str == "H") return KEY_H;
    if (str == "I") return KEY_I;
    if (str == "J") return KEY_J;
    if (str == "K") return KEY_K;
    if (str == "L") return KEY_L;
    if (str == "M") return KEY_M;
    if (str == "N") return KEY_N;
    if (str == "O") return KEY_O;
    if (str == "P") return KEY_P;
    if (str == "Q") return KEY_Q;
    if (str == "R") return KEY_R;
    if (str == "S") return KEY_S;
    if (str == "T") return KEY_T;
    if (str == "U") return KEY_U;
    if (str == "V") return KEY_V;
    if (str == "W") return KEY_W;
    if (str == "X") return KEY_X;
    if (str == "Y") return KEY_Y;
    if (str == "Z") return KEY_Z;
    if (str == "SLASH") return KEY_SLASH;
    if (str == "PERIOD") return KEY_PERIOD;
    if (str == "COMMA") return KEY_COMMA;
    if (str == "TAB") return KEY_TAB;
    if (str == "SPACE") return KEY_SPACE;
    if (str == "LEFT_BRACKET") return KEY_LEFT_BRACKET;
    if (str == "RIGHT_BRACKET") return KEY_RIGHT_BRACKET;
    if (str == "SPACE") return KEY_SPACE;
    if (str == "SEMICOLON") return KEY_SEMICOLON;
    if (str == "APOSTROPHE") return KEY_APOSTROPHE;
    if (str == "SLASH") return KEY_SLASH;
    if (str == "KP_0") return KEY_KP_0;
    if (str == "KP_1") return KEY_KP_1;
    if (str == "KP_2") return KEY_KP_2;
    if (str == "KP_3") return KEY_KP_3;
    if (str == "KP_4") return KEY_KP_4;
    if (str == "KP_5") return KEY_KP_5;
    if (str == "KP_6") return KEY_KP_6;
    if (str == "KP_7") return KEY_KP_7;
    if (str == "KP_8") return KEY_KP_8;
    if (str == "KP_9") return KEY_KP_9;
    if (str == "UP") return KEY_UP;
    if (str == "DOWN") return KEY_DOWN;
    if (str == "LEFT") return KEY_LEFT;
    if (str == "RIGHT") return KEY_RIGHT;
    // Add more as needed
    return -1;
}

void load_keybindings(string filename, shared_ptr<keybinding_list_t> kb) {
    minfo("Loading keybindings from file: %s", filename.c_str());
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        fprintf(
            stderr, "Failed to open keybindings file: %s\n", filename.c_str());
        exit(EXIT_FAILURE);
    }
    msuccess("Keybindings file opened successfully");
    char line[128];
    kb->count = 0;
    while (fgets(line, sizeof(line), file) &&
           kb->count < KEYBINDING_LIST_MAX_LENGTH) {
        massert(line, "Keybinding line is NULL");
        char keyname[64];
        char action[64];
        if (sscanf(line, " %63[^=] = %63s", keyname, action) == 2) {
            int key = key_from_string(keyname);
            if (key > 0) {
                kb->bindings[kb->count].key = key;
                strncpy(kb->bindings[kb->count].action,
                        action,
                        sizeof(kb->bindings[kb->count].action) - 1);
                kb->bindings[kb->count]
                    .action[sizeof(kb->bindings[kb->count].action) - 1] = '\0';
                kb->count++;
            } else {
                merror("Invalid key %s", keyname);
            }
        }
        bzero(line, sizeof(line));
    }
    fclose(file);
}

void print_keybindings(shared_ptr<keybinding_list_t> kb) {
    printf("Keybindings:\n");
    for (int i = 0; i < kb->count; i++) {
        printf("Key: %d, Action: %s\n",
               kb->bindings[i].key,
               kb->bindings[i].action);
    }
}
