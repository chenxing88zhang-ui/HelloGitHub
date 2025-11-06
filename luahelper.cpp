#include "luahelper.h"

LuaHelper::LuaHelper()
{
    lua = luaL_newstate();
    luaL_openlibs(lua);
    luaL_dostring(lua, "print('Hello from Lua!')");
}

LuaHelper::~LuaHelper()
{
    if (lua)
    {
        lua_close(lua);
        lua = 0;
    }
}

bool LuaHelper::ready(void) const
{
    return (lua != 0);
}
