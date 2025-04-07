#pragma once

//#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define KEYBINDING_MAX_ACTION_LENGTH 32
#define KEYBINDING_LIST_MAX_LENGTH 400

typedef struct {
    int key;
    char action[KEYBINDING_MAX_ACTION_LENGTH];
} keybinding_t;

typedef struct {
    keybinding_t bindings[KEYBINDING_LIST_MAX_LENGTH];
    int count;
} keybinding_list_t;

const char* get_action_for_key(keybinding_list_t* kb, int key);
int key_from_string(const char* str);
void load_keybindings(const char* filename, keybinding_list_t* kb);
