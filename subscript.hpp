#ifndef SUBSCRIPT_HPP
#define SUBSCRIPT_HPP

#include <QThread>

#include <iostream>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
    #include "luajit.h"
}

class SubScript : public QThread {
    Q_OBJECT
public:
    SubScript(lua_State *L_main) {
        L = luaL_newstate();
        // FIXME check for failure?
        lua_pushlightuserdata(L, this);
        lua_rawseti(L, LUA_REGISTRYINDEX, 0);
        luaL_openlibs(L);
        int err = luaL_loadstring(L, lua_tostring(L_main, 1));
        if (err) {
            std::cout << "Error in subscript: " << err << std::endl;
            std::cout << lua_tostring(L, -1) << std::endl;
        }
        for (int stackpos = 4;stackpos <= lua_gettop(L_main);stackpos++) {
            switch (lua_type(L_main, stackpos)) {
            case LUA_TNIL:
                lua_pushnil(L);
                break;
            case LUA_TBOOLEAN:
                lua_pushboolean(L, lua_toboolean(L_main, stackpos));
                break;
            case LUA_TNUMBER:
                lua_pushnumber(L, lua_tonumber(L_main, stackpos));
                break;
            case LUA_TSTRING:
                lua_pushstring(L, lua_tostring(L_main, stackpos));
                break;
            }
        }
        lua_pushnumber(L, lua_gettop(L_main) - 3);
    }
    
    void run() override {
        std::cout << "Thread running!" << std::endl;
        int numarg = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
        std::cout << numarg << ", " << lua_gettop(L) << std::endl;
        if (lua_pcall(L, numarg, LUA_MULTRET, 0)) {
            std::cout << "Error in thread call: " << lua_tostring(L, -1) << std::endl;
        }
    };

    void onSubFinished(lua_State *L_main, int id) {
        lua_getfield(L_main, LUA_REGISTRYINDEX, "uicallbacks");
        lua_getfield(L_main, -1, "MainObject");
        lua_getfield(L_main, -1, "OnSubFinished");
        lua_insert(L_main, -2);
        lua_pushinteger(L_main, id);
        int n = lua_gettop(L);
        for (int i = 1; i <= n; i++) {
            switch (lua_type(L, i)) {
            case LUA_TNIL:
                lua_pushnil(L_main);
                std::cout << "pushnil" << std::endl;
                break;
            case LUA_TBOOLEAN:
                lua_pushboolean(L_main, lua_toboolean(L, i));
                std::cout << "pushbool" << std::endl;
                break;
            case LUA_TNUMBER:
                lua_pushnumber(L_main, lua_tonumber(L, i));
                std::cout << "pushnum" << std::endl;
                break;
            case LUA_TSTRING:
                lua_pushstring(L_main, lua_tostring(L, i));
                std::cout << "pushstr" << std::endl;
                std::cout << lua_tostring(L, i) << std::endl;
                break;
            default:
                std::cout << "Subscript return " << (i - 1) << ": only nil, boolean, number and string can be returned from sub script" << std::endl;
                return;
            }
        }
        std::cout << "ID is " << id << std::endl;
        std::cout << "onsubfinished main top: " << lua_gettop(L_main) << " thread top: " << lua_gettop(L) << std::endl;
        std::cout << lua_type(L, -1) << " " << LUA_TNIL << " " << LUA_TBOOLEAN << " " << LUA_TNUMBER << " " << LUA_TSTRING << " " << LUA_TTABLE << std::endl;
        int result = lua_pcall(L_main, lua_gettop(L) + 2, 0, 0);
        if (result) {
            std::cout << "Error calling OnSubFinished: " << result << std::endl;
            std::cout << lua_tostring(L_main, -1) << std::endl;
        }
    }
    lua_State *L;
};

#endif
