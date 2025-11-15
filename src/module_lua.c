/*======================================================================
  module_lua.c  –  tiny Lua bridge (fixed linkage)
======================================================================*/

#include "module_lua.h"
#include "sokol_app.h"
#include <stdio.h>
#include "cimgui.h"
// #include "sokol_imgui.h"

/* ------------------------------------------------------------------ */
/*  Global Lua state (file-scope, not static inside function)         */
/* ------------------------------------------------------------------ */
static lua_State *L = NULL;

/* ------------------------------------------------------------------ */
/*  C → Lua : test_call()  →  hello_world()                           */
/* ------------------------------------------------------------------ */
int test_call(lua_State *L)
{
    (void)L;
    printf("Hello World from C! (called from Lua)\n");
    return 0;
}


static int lua_sapp_frame_count(lua_State *L)
{
    lua_pushinteger(L, sapp_frame_count());
    return 1;
}


/* ------------------------------------------------------------------ */
/*  Initialise Lua, register function and load script.lua             */
/* ------------------------------------------------------------------ */
void lua_module_init(void)
{
    // Close previous state if any (for hot-reload, optional)
    if (L) {
        lua_close(L);
    }

    L = luaL_newstate();
    if (!L) {
        fprintf(stderr, "ERROR: cannot create Lua state\n");
        return;
    }

    luaL_openlibs(L);

    // Register C function as hello_world()
    lua_pushcfunction(L, test_call);lua_setglobal(L, "hello_world");
    lua_pushcfunction(L, lua_sapp_frame_count);lua_setglobal(L, "sapp_frame_count");

    // Load script.lua
    // const char *script = "script.lua";
    // if (luaL_dofile(L, script) != LUA_OK) {
    //     fprintf(stderr, "ERROR loading %s: %s\n",
    //             script, lua_tostring(L, -1));
    //     lua_pop(L, 1);
    // }
}

lua_State* get_lua_state(void)
{
    return L;
}


/* ------------------------------------------------------------------ */
/*  2. Load and run a script if the file exists                       */
/* ------------------------------------------------------------------ */
int load_script(const char *filename)
{
    if (!L) {
        fprintf(stderr, "Lua state not initialized\n");
        return -1;
    }

    // Check if file exists
    FILE *f = fopen(filename, "rb");
    if (!f) {
        // Not an error — just skip
        printf("Lua: file '%s' not found — skipped\n", filename);
        return 0;
    }
    fclose(f);

    // Load and execute
    if (luaL_dofile(L, filename) != LUA_OK) {
        fprintf(stderr, "Lua ERROR in %s: %s\n",
                filename, lua_tostring(L, -1));
        lua_pop(L, 1);
        return -1;
    }

    printf("Lua: '%s' loaded successfully\n", filename);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Helper: call Lua function hello_world() from C                    */
/* ------------------------------------------------------------------ */
static void call_lua_hello(void)
{
    if (!L) {
        fprintf(stderr, "Lua state not initialized\n");
        return;
    }

    lua_getglobal(L, "hello_world");
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    } else {
        lua_pop(L, 1);
        fprintf(stderr, "Lua: hello_world is not a function\n");
    }
}

/* ------------------------------------------------------------------ */
/*  Public: draw button and trigger Lua → C call                      */
/* ------------------------------------------------------------------ */
void lua_module_frame(void)
{
    // if (igButton("Call Lua to C", (ImVec2){0, 0})) {
    if (igButton("Call Lua to C")) {
    // if (igButton("Call Lua to C",(struct ImVec2){0, 0})) {
        call_lua_hello();
    }
}

/* ------------------------------------------------------------------ */
/*  Optional: cleanup on exit                                         */
/* ------------------------------------------------------------------ */
void lua_module_shutdown(void)
{
    if (L) {
        lua_close(L);
        L = NULL;
    }
}