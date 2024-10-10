#pragma once

// Lua support
#ifndef WEB
#include <lua5.4/lauxlib.h>
#include <lua5.4/lua.h>
#include <lua5.4/luaconf.h>
#include <lua5.4/lualib.h>
#else
#include <lauxlib.h>
#include <lua.h>
#include <luaconf.h>
#include <lualib.h>
#endif
