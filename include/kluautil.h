#pragma once
#include "kluaconv.h"
#include "k/lauxlib.h"
#include <type_traits>

namespace klua {

template <typename K, typename V>
inline void table_set(lua_State* L, K&& k, V&& v)
{
    to_lua(L, std::forward<K>(k));
    to_lua(L, std::forward<V>(v));
    lua_rawset(L, -3);
}

template <typename K, typename V>
inline bool table_get(lua_State* L, K&& k, V& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    to_lua(L, std::forward<K>(k));
    lua_rawget(L, -2);
    if (from_lua(L, v))
        return true;
    lua_pop(L, 1);
    return false;
}

inline void create_metatable(lua_State* L, const char* name, const luaL_Reg* funcs)
{
    luaL_newmetatable(L, name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, funcs, 0);
    lua_pop(L, 1);
}

} // namespace klua
