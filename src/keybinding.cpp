#include "keybinding.h"
#include "massert.h"
#include "mprint.h"
#include "raylib.h"

const char* get_action_for_key(keybinding_list_t* kb, int key) {
    //minfo("Getting action for key %d", key);
    massert(kb, "Keybinding list is NULL");
    massert(kb->count > 0, "Keybinding list is empty");
    massert(key >= 0, "Key is less than 0");
    for (int i = 0; i < kb->count; i++) {
        if (kb->bindings[i].key == key) {
            //minfo("Key %d found in keybinding list", key);
            return kb->bindings[i].action;
        }
    }
    //minfo("Key %d not found in keybinding list", key);
    return NULL;
}

int key_from_string(const char* str) {
    //minfo("Converting string to key [%s]", str);
    //if (!strncmp(str, "W", )) return KEY_W;
    if (!strncmp(str, "ESCAPE", 2)) return KEY_ESCAPE;
    if (!strncmp(str, "A", 2)) return KEY_A;
    if (!strncmp(str, "B", 2)) return KEY_B;
    if (!strncmp(str, "C", 2)) return KEY_C;
    if (!strncmp(str, "D", 2)) return KEY_D;
    if (!strncmp(str, "E", 2)) return KEY_E;
    if (!strncmp(str, "F", 2)) return KEY_F;
    if (!strncmp(str, "G", 2)) return KEY_G;
    if (!strncmp(str, "H", 2)) return KEY_H;
    if (!strncmp(str, "I", 2)) return KEY_I;
    if (!strncmp(str, "J", 2)) return KEY_J;
    if (!strncmp(str, "K", 2)) return KEY_K;
    if (!strncmp(str, "L", 2)) return KEY_L;
    if (!strncmp(str, "M", 2)) return KEY_M;
    if (!strncmp(str, "N", 2)) return KEY_N;
    if (!strncmp(str, "O", 2)) return KEY_O;
    if (!strncmp(str, "P", 2)) return KEY_P;
    if (!strncmp(str, "Q", 2)) return KEY_Q;
    if (!strncmp(str, "R", 2)) return KEY_R;
    if (!strncmp(str, "S", 2)) return KEY_S;
    if (!strncmp(str, "T", 2)) return KEY_T;
    if (!strncmp(str, "U", 2)) return KEY_U;
    if (!strncmp(str, "V", 2)) return KEY_V;
    if (!strncmp(str, "W", 2)) return KEY_W;
    if (!strncmp(str, "X", 2)) return KEY_X;
    if (!strncmp(str, "Y", 2)) return KEY_Y;
    if (!strncmp(str, "Z", 2)) return KEY_Z;
    if (!strncmp(str, "SLASH", 6)) return KEY_SLASH;
    if (!strncmp(str, "PERIOD", 7)) return KEY_PERIOD;
    if (!strncmp(str, "COMMA", 6)) return KEY_COMMA;
    if (!strncmp(str, "TAB", 4)) return KEY_TAB;
    if (!strncmp(str, "SPACE", 6)) return KEY_SPACE;
    if (!strncmp(str, "LEFT_BRACKET", 6)) return KEY_LEFT_BRACKET;
    if (!strncmp(str, "RIGHT_BRACKET", 6)) return KEY_RIGHT_BRACKET;
    if (!strncmp(str, "SPACE", 6)) return KEY_SPACE;
    if (!strncmp(str, "SEMICOLON", 6)) return KEY_SEMICOLON;
    if (!strncmp(str, "APOSTROPHE", 6)) return KEY_APOSTROPHE;
    if (!strncmp(str, "SLASH", 6)) return KEY_SLASH;
    if (!strncmp(str, "KP_0", 5)) return KEY_KP_0;
    if (!strncmp(str, "KP_1", 5)) return KEY_KP_1;
    if (!strncmp(str, "KP_2", 5)) return KEY_KP_2;
    if (!strncmp(str, "KP_3", 5)) return KEY_KP_3;
    if (!strncmp(str, "KP_4", 5)) return KEY_KP_4;
    if (!strncmp(str, "KP_5", 5)) return KEY_KP_5;
    if (!strncmp(str, "KP_6", 5)) return KEY_KP_6;
    if (!strncmp(str, "KP_7", 5)) return KEY_KP_7;
    if (!strncmp(str, "KP_8", 5)) return KEY_KP_8;
    if (!strncmp(str, "KP_9", 5)) return KEY_KP_9;
    if (!strncmp(str, "UP", 3)) return KEY_UP;
    if (!strncmp(str, "DOWN", 5)) return KEY_DOWN;
    if (!strncmp(str, "LEFT", 5)) return KEY_LEFT;
    if (!strncmp(str, "RIGHT", 6)) return KEY_RIGHT;
    // Add more as needed
    return -1;
}

void load_keybindings(const char* filename, keybinding_list_t* kb) {
    //minfo("Loading keybindings from %s", filename);
    FILE* file = fopen(filename, "r");
    if (!file) return;
    msuccess("Keybindings file opened successfully");
    char line[128];
    kb->count = 0;
    //minfo("Parsing keybindings file");
    while (fgets(line, sizeof(line), file) && kb->count < KEYBINDING_LIST_MAX_LENGTH) {
        //minfo("Parsing line %s", line);
        massert(line, "Keybinding line is NULL");
        char keyname[64], action[64];
        if (sscanf(line, " %63[^=] = %63s", keyname, action) == 2) {
            int key = key_from_string(keyname);
            if (key > 0) {
                //minfo("Key %s", keyname);
                //minfo("Action %s", action);
                kb->bindings[kb->count].key = key;
                strncpy(kb->bindings[kb->count].action, action, sizeof(kb->bindings[kb->count].action) - 1);
                kb->bindings[kb->count].action[sizeof(kb->bindings[kb->count].action) - 1] = '\0';
                kb->count++;
            } else {
                merror("Invalid key %s", keyname);
            }
        }
        bzero(line, sizeof(line));
    }

    fclose(file);
}
