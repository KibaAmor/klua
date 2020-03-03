#include "usermgr.h"
#include <cassert>
using namespace klua;


static const char* const USERMGR_METATABLE = "mt.usermgr";

#define tousermgr(L)	(static_cast<usermgr**>(luaL_checkudata(L, 1, USERMGR_METATABLE)))

static int usermgr_get_user_count(lua_State* L)
{
    usermgr** u = tousermgr(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user manager");
    to_lua(L, (*u)->get_user_count());
    return 1;
}

static const luaL_Reg flib[] =
{
    {"get_user_count", usermgr_get_user_count},
    {nullptr, nullptr},
};

void reg_usermgr_to_lua(const klua::lua& l, usermgr& um)
{
    lua_State* L = static_cast<lua_State*>(l);

    create_metatable(L, USERMGR_METATABLE, flib);

    usermgr** u = static_cast<usermgr**>(lua_newuserdata(L, sizeof(usermgr*)));
    assert(nullptr != u);
    *u = &um;

    luaL_setmetatable(L, USERMGR_METATABLE);

    lua_setglobal(L, "usermgr");
}
