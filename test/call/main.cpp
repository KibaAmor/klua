#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"
#include <climits>
#include <cfloat>
#include <klua.h>
#include "msg.h"
#include "user.h"
#include "usermgr.h"
using namespace klua;


struct kom
{
    std::string name;
    int32_t age;
    bool sex = true;
    double salary = 0.0;
    int64_t data = 0;

    std::string to_string() const
    {
        return std::string("name:") + name
            + ", age:" + std::to_string(age)
            + ", sex:" + (sex ? "male" : "female")
            + ", salary:" + std::to_string(salary)
            + ", data:" + std::to_string(data);
    }
};

void to_lua(lua_State* L, const kom& v)
{
    lua_newtable(L);
    table_set(L, "name", v.name);
    table_set(L, "age", v.age);
    table_set(L, "sex", v.sex);
    table_set(L, "salary", v.salary);
    table_set(L, "data", v.data);
}

bool from_lua(lua_State* L, kom& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;

    if (table_get(L, "name", v.name)
        && table_get(L, "age", v.age)
        && table_get(L, "sex", v.sex)
        && table_get(L, "salary", v.salary)
        && table_get(L, "data", v.data))
    {
        lua_pop(L, 1);
        return true;
    }
    return false;
}

const char* ls = R"(
function dump_kom(k)
    klua.dump(k, "k")
    print(k.name)
    print(k.age)
    print(k.sex)
    print(k.salary)
    print(k.data)
    return k
end

local submsg1 = {name="kiba", age=18}
local submsg2 = {name="amor", age=28}
assert(msg.dump_submsg(submsg1))
assert(msg.dump_submsg(submsg2))

local msg1 = {name="team1", submsgs={submsg1, submsg2}}
assert(msg.dump_msg(msg1))

local u = user.new()
u:set_account("kaccount")
print(u:get_account())
u:set_nickname("knickname")
print(u:get_nickname())
u:set_userid(8888)
print(u:get_userid())

print("user count:"..usermgr:get_user_count())
u = nil
collectgarbage()
print("user count:"..usermgr:get_user_count())
)";

TEST_SUITE_BEGIN("userdata");

TEST_CASE("call")
{
    lua l;
    CHECK(l.gettop() == 0);
    l.load_stdlibs();
    CHECK(l.gettop() == 0);
    reg_msg_to_lua(l);
    CHECK(l.gettop() == 0);
    reg_user_to_lua(l);
    CHECK(l.gettop() == 0);

    usermgr& um = usermgr::instance();
    reg_usermgr_to_lua(l, um);
    CHECK(l.gettop() == 0);

    kom k;
    k.name = "kom";
    k.age = 18;
    k.sex = true;
    k.salary = DBL_MAX;
    k.data = LLONG_MAX;
    kom m;

    CHECK(l.dostring(ls));
    CHECK(l.gettop() == 0);

    CHECK(l.call1p("dump_kom", k, m));
    CHECK(m.name == k.name);
    CHECK(m.age == k.age);
    CHECK(m.sex == k.sex);
    CHECK(m.salary == k.salary);
    CHECK(m.data == k.data);
    CHECK(l.gettop() == 0);

    CHECK(!l.call1p("error.test", k)); // false
    CHECK(l.gettop() == 0);

    CHECK(l.call1p("klua.dump", k));
    CHECK(l.gettop() == 0);

    CHECK(l.call2p("klua.dump", k, "k"));
    CHECK(l.gettop() == 0);
}

TEST_SUITE_END();
