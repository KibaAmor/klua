#pragma once
#include <klua.h>


struct submsg
{
    std::string name;
    uint8_t age = 0;

    std::string to_string() const
    {
        return std::string("name:") + name
            + ", age:" + std::to_string(age);
    }
};

void to_lua(lua_State* L, const submsg& v);
bool from_lua(lua_State* L, submsg& v);

struct msg
{
    std::string name;
    std::vector<submsg> submsgs;

    std::string to_string() const
    {
        std::string str = std::string("name:") + name + ", submsgs:";
        for (const auto& s : submsgs)
            str += "[" + s.to_string() + "]";
        return str;
    }
};

void to_lua(lua_State* L, const msg& v);
bool from_lua(lua_State* L, msg& v);

void reg_msg_to_lua(klua::lua& l);
