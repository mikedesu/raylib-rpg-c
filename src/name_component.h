#pragma once
#include "entityid.h"
#include <string>
using std::string;
typedef struct name_component_t {
    entityid id;
    //char name[32];
    string name;
} name_component;
