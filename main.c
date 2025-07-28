#include <stdio.h>
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define FILE_OPEN_FAIL -1
#define SUCCESS 0
#define GENERIC_FAIL 1
#define MALLOC_FAIL 2
#define REALLOC_FAIL 3

int main(void) {
    FILE *luaFile = fopen("main.lua", "rb");
    if (!luaFile) {
        fprintf(stderr, "Error: %d", FILE_OPEN_FAIL);
        exit(1);
    }
    fseek(luaFile, 0, SEEK_END);
    long fileSize = ftell(luaFile);
    fseek(luaFile, 0, SEEK_SET);
    char *luaCode = (char *)malloc(fileSize+1);
    if (!luaCode) {
        fprintf(stderr, "Error: %d", MALLOC_FAIL);
        exit(1);
    }
    fread(luaCode, fileSize, 1, luaFile);
    fclose(luaFile);
    luaCode[fileSize] = 0;
    
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadstring(L, luaCode) == LUA_OK) {
        if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
            lua_pop(L, lua_gettop(L));
        }
    }

    free(luaCode);
    lua_close(L);
    return SUCCESS;
}