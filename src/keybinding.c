#include "keybinding.h"
#include "massert.h"
#include "mprint.h"
#include "raylib.h"

const char* get_action_for_key(keybinding_list_t* kb, int key) {
    for (int i = 0; i < kb->count; i++) {
        if (kb->bindings[i].key == key) { return kb->bindings[i].action; }
    }
    return NULL;
}

int key_from_string(const char* str) {
    minfo("Converting string to key [%s]", str);

    if (!strcmp(str, "W")) return KEY_W;
    if (!strcmp(str, "A")) return KEY_A;
    if (!strcmp(str, "S")) return KEY_S;
    if (!strcmp(str, "D")) return KEY_D;
    if (!strcmp(str, "Q")) return KEY_Q;
    if (!strcmp(str, "E")) return KEY_E;
    if (!strcmp(str, "Z")) return KEY_Z;
    if (!strcmp(str, "C")) return KEY_C;
    if (!strcmp(str, "SLASH")) return KEY_SLASH;
    if (!strcmp(str, "PERIOD")) return KEY_PERIOD;
    if (!strcmp(str, "COMMA")) return KEY_COMMA;
    if (!strcmp(str, "TAB")) return KEY_TAB;
    if (!strcmp(str, "SPACE")) return KEY_SPACE;
    if (!strcmp(str, "KP_0")) return KEY_KP_0;
    if (!strcmp(str, "KP_1")) return KEY_KP_1;
    if (!strcmp(str, "KP_2")) return KEY_KP_2;
    if (!strcmp(str, "KP_3")) return KEY_KP_3;
    if (!strcmp(str, "KP_4")) return KEY_KP_4;
    if (!strcmp(str, "KP_5")) return KEY_KP_5;
    if (!strcmp(str, "KP_6")) return KEY_KP_6;
    if (!strcmp(str, "KP_7")) return KEY_KP_7;
    if (!strcmp(str, "KP_8")) return KEY_KP_8;
    if (!strcmp(str, "KP_9")) return KEY_KP_9;
    // Add more as needed
    return -1;
}

void load_keybindings(const char* filename, keybinding_list_t* kb) {
    minfo("Loading keybindings from %s", filename);
    FILE* file = fopen(filename, "r");
    if (!file) return;

    msuccess("Keybindings file opened successfully");

    char line[128];
    kb->count = 0;

    minfo("Parsing keybindings file");
    while (fgets(line, sizeof(line), file) && kb->count < KEYBINDING_LIST_MAX_LENGTH) {
        minfo("Parsing line %s", line);
        massert(line, "Keybinding line is NULL");
        char keyname[64], action[64];
        if (sscanf(line, " %63[^=] = %63s", keyname, action) == 2) {
            int key = key_from_string(keyname);
            if (key > 0) {
                minfo("Key %s", keyname);
                minfo("Action %s", action);
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
