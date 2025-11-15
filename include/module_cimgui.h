#pragma once
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* ------------------------------------------------------------------ */
/*  Initialise the cimgui-Lua bridge (called once after lua_module_init)*/
/* ------------------------------------------------------------------ */
void cimgui_module_init(lua_State *L);

/* ------------------------------------------------------------------ */
/*  Called every frame – draws the UI that lives in Lua (_render)      */
/* ------------------------------------------------------------------ */
void cimgui_module_frame(void);