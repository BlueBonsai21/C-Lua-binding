#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

//! ERROR CODES
#define FILE_OPEN_FAIL "FILE OPEN FAIL"
#define GENERIC_FAIL "GENERIC FAIL"
#define MALLOC_FAIL "MALLOC FAIL"
#define REALLOC_FAIL "REALLOC FAIL"

#define throw_error(error) {\
fprintf(stderr, "%s (F: %s, L:%i)\n", error, __FILE__, __LINE__);\
exit(1);}

int main(void) {
    FILE *luaFile = fopen("main.lua", "rb");
    if (!luaFile) throw_error(FILE_OPEN_FAIL);
    
    fseek(luaFile, 0, SEEK_END);
    long fileSize = ftell(luaFile);
    fseek(luaFile, 0, SEEK_SET);

    char *luaCode = malloc(fileSize+1);
    if (!luaCode) throw_error(MALLOC_FAIL);

    fread(luaCode, fileSize, 1, luaFile);
    fclose(luaFile);
    luaCode[fileSize] = '\0';
    
    lua_State *L = luaL_newstate();
    if (!L) {
        free(luaCode);
        lua_close(L);
        return 1;
    }
    
    luaL_openlibs(L);
    
    do {
        if (luaL_loadstring(L, luaCode) != LUA_OK) throw_error(lua_tostring(L, -1));
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) throw_error(lua_tostring(L, -1));
        lua_pop(L, lua_gettop(L));

        luaFile = fopen("main.lua", "rb");
        if (!luaFile) throw_error(FILE_OPEN_FAIL);
    
        fseek(luaFile, 0, SEEK_END);
        long fileSize = ftell(luaFile);
        fseek(luaFile, 0, SEEK_SET);

        luaCode = malloc(fileSize+1);
        if (!luaCode) throw_error(MALLOC_FAIL);

        fread(luaCode, fileSize, 1, luaFile);
        fclose(luaFile);
        luaCode[fileSize] = '\0';
    } while (true);

    free(luaCode);
    lua_close(L);
    
    return 0;
}