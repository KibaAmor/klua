#include "user.h"
#include "usermgr.h"
#include <cassert>
#include <iostream>
using namespace klua;


user::user()
{
    usermgr::instance().inc_user_count();
    std::cout << "user::user()" << std::endl;
}

user::~user()
{
    std::cout << "user::~user()" << std::endl;
    usermgr::instance().dec_user_count();
}

void to_lua(lua_State* L, const user& v)
{
    lua_newtable(L);
    table_set(L, "account", v._account);
    table_set(L, "nickname", v._nickname);
    table_set(L, "userid", v._userid);
}

bool from_lua(lua_State* L, user& v)
{

    if (table_get(L, "account", v._account)
        && table_get(L, "nickname", v._nickname)
        && table_get(L, "userid", v._userid))
    {
        lua_pop(L, 1);
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------

static const char* const USER_METATABLE = "mt.user";

#define touser(L)	(static_cast<user**>(luaL_checkudata(L, 1, USER_METATABLE)))

static int user_get_account(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    to_lua(L, (*u)->get_account());
    return 1;
}

static int user_set_account(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    size_t l = 0;
    const char* s = luaL_checklstring(L, -1, &l);
    std::string str;
    if (l > 0)
        str.assign(s, s + l);
    (*u)->set_account(str);
    return 0;
}

static int user_get_nickname(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    to_lua(L, (*u)->get_nickname());
    return 1;
}

static int user_set_nickname(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    size_t l = 0;
    const char* s = luaL_checklstring(L, -1, &l);
    std::string str;
    if (l > 0)
        str.assign(s, s + l);
    (*u)->set_nickname(str);
    return 0;
}

static int user_get_userid(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    to_lua(L, (*u)->get_userid());
    return 1;
}

static int user_set_userid(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    const auto n = luaL_checknumber(L, -1);
    (*u)->set_userid(static_cast<decltype((*u)->get_userid())>(n));
    return 0;
}

static int user_gc(lua_State* L)
{
    user** u = touser(L);
    luaL_argcheck(L, nullptr != u && nullptr != (*u), 1, "invalid user");
    delete (*u);
    return 0;
}

static const luaL_Reg flib[] =
{
    {"get_account", user_get_account},
    {"set_account", user_set_account},
    {"get_nickname", user_get_nickname},
    {"set_nickname", user_set_nickname},
    {"get_userid", user_get_userid},
    {"set_userid", user_set_userid},
    {"__gc", user_gc},
    {nullptr, nullptr},
};


static int user_new(lua_State* L)
{
    user** u = static_cast<user**>(lua_newuserdata(L, sizeof(user*)));
    assert(nullptr != u);
    *u = new user();
    luaL_setmetatable(L, USER_METATABLE);
    return 1;
}

static const luaL_Reg userlib[] =
{
    {"new", user_new},
    {nullptr, nullptr},
};

static int luaopen_user(lua_State *L)
{
    luaL_newlib(L, userlib);
    create_metatable(L, USER_METATABLE, flib);
    return 1;
}

void reg_user_to_lua(klua::lua& l)
{
    l.requiref("user", luaopen_user, true);
}
