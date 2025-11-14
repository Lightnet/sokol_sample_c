#pragma once
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* Initialise Lua, load script.lua and expose our C function */
void lua_module_init(void); // create state, open libs
int  load_script(const char *filename);  // load+run if file exists

/* The C function that Lua will call */
int test_call(lua_State *L);
void lua_module_frame(void); // ImGui button

void lua_module_shutdown(void);  // clean up