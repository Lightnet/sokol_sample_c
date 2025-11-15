/*======================================================================
  module_cimgui.c  –  Dear-ImGui bridge for Lua
======================================================================*/

#include "module_cimgui.h"
#include "cimgui.h"
#include <stdio.h>
#include <string.h>


static lua_State *L = NULL;               // same global state as module_lua.c

/* ------------------------------------------------------------------ */
/*  Helper: push a C function onto the Lua stack and register it      */
/* ------------------------------------------------------------------ */
static void register_cfunc(const char *name, lua_CFunction fn)
{
    lua_pushcfunction(L, fn);
    lua_setglobal(L, name);
}


/* --------------------------------------------------------------- */
/*  SetNextWindowSize( {w,h}, cond )                               */
/* --------------------------------------------------------------- */
static int cimgui_SetNextWindowSize(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    // Try named fields: x or w
    lua_getfield(L, 1, "x"); float w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    if (w == 0) {
        lua_getfield(L, 1, "w"); w = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    if (w == 0) { lua_rawgeti(L, 1, 1); w = (float)luaL_checknumber(L, -1); lua_pop(L, 1); }

    // Height: y or h or index 2
    lua_getfield(L, 1, "y"); float h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    if (h == 0) {
        lua_getfield(L, 1, "h"); h = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    }
    if (h == 0) { lua_rawgeti(L, 1, 2); h = (float)luaL_checknumber(L, -1); lua_pop(L, 1); }

    // Condition
    ImGuiCond cond = ImGuiCond_Always;
    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
        const char *s = lua_tostring(L, 2);
        if (strcmp(s, "Once") == 0)           cond = ImGuiCond_Once;
        else if (strcmp(s, "FirstUseEver") == 0) cond = ImGuiCond_FirstUseEver;
        else if (strcmp(s, "Appearing") == 0)    cond = ImGuiCond_Appearing;
    }

    igSetNextWindowSize((ImVec2){w, h}, cond);
    return 0;
}

/* --------------------------------------------------------------- */
/*  SetNextWindowPos( {x,y}, cond )                                */
/* --------------------------------------------------------------- */
static int cimgui_SetNextWindowPos(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    // X: try "x" or index 1
    lua_getfield(L, 1, "x"); float x = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    if (x == 0) { lua_rawgeti(L, 1, 1); x = (float)luaL_checknumber(L, -1); lua_pop(L, 1); }

    // Y: try "y" or index 2
    lua_getfield(L, 1, "y"); float y = (float)lua_tonumber(L, -1); lua_pop(L, 1);
    if (y == 0) { lua_rawgeti(L, 1, 2); y = (float)luaL_checknumber(L, -1); lua_pop(L, 1); }

    // Condition
    ImGuiCond cond = ImGuiCond_Always;
    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
        const char *s = lua_tostring(L, 2);
        if (strcmp(s, "Once") == 0)           cond = ImGuiCond_Once;
        else if (strcmp(s, "FirstUseEver") == 0) cond = ImGuiCond_FirstUseEver;
        else if (strcmp(s, "Appearing") == 0)    cond = ImGuiCond_Appearing;
    }

    igSetNextWindowPos((ImVec2){x, y}, cond);  // Only 2 args!
    return 0;
}


/* ------------------------------------------------------------------ */
/*  Minimal wrappers for the Dear-ImGui functions you’ll need in Lua  */
/* ------------------------------------------------------------------ */
static int cimgui_Begin(lua_State *L)
{
    const char *title = luaL_checkstring(L, 1);
    bool open = igBegin(title, NULL, ImGuiWindowFlags_None);
    lua_pushboolean(L, open);
    return 1;
}

static int cimgui_End(lua_State *L)
{
    (void)L;
    igEnd();
    return 0;
}

static int cimgui_Button(lua_State *L)
{
    const char *label = luaL_checkstring(L, 1);
    bool clicked = igButton(label);
    lua_pushboolean(L, clicked);
    return 1;
}

static int cimgui_Text(lua_State *L)
{
    const char *text = luaL_checkstring(L, 1);
    igText("%s", text);  // igText supports printf-style
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Initialise the bridge – called once after the Lua state exists    */
/* ------------------------------------------------------------------ */
void cimgui_module_init(lua_State *L_in)
{
    L = L_in;                     // keep the same global state

    /* create a global table `imgui` and fill it with wrappers */
    lua_newtable(L);

    /* window */
    lua_pushcfunction(L, cimgui_Begin);  lua_setfield(L, -2, "BeginWindow");
    lua_pushcfunction(L, cimgui_End);    lua_setfield(L, -2, "EndWindow");// end keyword is use by lua
    

    /* widgets */
    lua_pushcfunction(L, cimgui_Button); lua_setfield(L, -2, "Button"); 
    lua_pushcfunction(L, cimgui_Text); lua_setfield(L, -2, "Text");
    lua_pushcfunction(L, cimgui_SetNextWindowSize); lua_setfield(L, -2, "SetNextWindowSize");
    lua_pushcfunction(L, cimgui_SetNextWindowPos);  lua_setfield(L, -2, "SetNextWindowPos");

    lua_setglobal(L, "imgui");
}

/* ------------------------------------------------------------------ */
/*  Frame – call Lua _render() if it exists                           */
/* ------------------------------------------------------------------ */
void cimgui_module_frame(void)
{
    if (!L) return;

    lua_getglobal(L, "_render");          // push _render or nil
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            fprintf(stderr, "Lua _render error: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    } else {
        lua_pop(L, 1);                    // remove nil
        /* No _render → draw a tiny fallback so the window is never empty */
        igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once);
        igSetNextWindowSize((ImVec2){400, 100}, ImGuiCond_Once);
        igBegin("Lua UI missing", NULL, ImGuiWindowFlags_None);
        igText("Add a function _render() in your script.lua");
        igEnd();
    }
}

