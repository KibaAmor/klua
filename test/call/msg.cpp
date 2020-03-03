#include "msg.h"
#include <iostream>
using namespace klua;


void to_lua(lua_State* L, const submsg& v)
{
    lua_newtable(L);
    table_set(L, "name", v.name);
    table_set(L, "age", v.age);
}

bool from_lua(lua_State* L, submsg& v)
{
    if (table_get(L, "name", v.name)
        && table_get(L, "age", v.age))
    {
        lua_pop(L, 1);
        return true;
    }
    return false;
}


void to_lua(lua_State* L, const msg& v)
{
    lua_newtable(L);
    table_set(L, "name", v.name);
    table_set(L, "submsgs", v.submsgs);
}

bool from_lua(lua_State* L, msg& v)
{
    if (table_get(L, "name", v.name)
        && table_get(L, "submsgs", v.submsgs))
    {
        lua_pop(L, 1);
        return true;
    }
    return false;
}

static int data_dump_submsg(lua_State *L)
{
    submsg v;
    to_lua(L, from_lua(L, v));
    std::cout << "dump_submsg:" << v.to_string() << std::endl;
    return 1;
}

static int data_dump_msg(lua_State *L)
{
    msg v;
    to_lua(L, from_lua(L, v));
    std::cout << "dump_msg:" << v.to_string() << std::endl;
    return 1;
}

static const luaL_Reg datalib[] =
{
    { "dump_submsg", data_dump_submsg},
    { "dump_msg", data_dump_msg},
    { nullptr, nullptr },
};

int luaopen_data(lua_State *L)
{
    luaL_newlib(L, datalib);
    return 1;
}

void reg_msg_to_lua(klua::lua& l)
{
    l.requiref("msg", luaopen_data, true);
}
