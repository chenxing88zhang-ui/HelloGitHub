#ifndef LUAHELPER_H
#define LUAHELPER_H

#include <string>
#include <vector>
#include <map>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

class LuaHelper
{

public:
    explicit LuaHelper();
    ~LuaHelper();

    bool ready(void) const;

private:
    lua_State* lua ;
};

#endif // LUAHELPER_H
