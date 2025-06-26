#pragma once

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

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

const char* get_action_for_key(std::shared_ptr<keybinding_list_t> kb, int key);
int key_from_string(std::string str);
void load_keybindings(std::string filename, std::shared_ptr<keybinding_list_t> kb);
void print_keybindings(std::shared_ptr<keybinding_list_t> kb);
