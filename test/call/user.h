#pragma once
#include <klua.h>


class user final
{
public:
    user();
    ~user();

    const std::string& get_account() const noexcept { return _account; }
    void set_account(const std::string& account) { _account = account; }

    const std::string& get_nickname() const noexcept { return _nickname; }
    void set_nickname(const std::string& nickname) { _nickname = nickname; }

    uint64_t get_userid() const noexcept { return _userid; }
    void set_userid(uint64_t userid) noexcept { _userid = userid; }

private:
    friend void to_lua(lua_State* L, const user& v);
    friend bool from_lua(lua_State* L, user& v);

    std::string _account;
    std::string _nickname;
    uint64_t _userid = 0;
};

void to_lua(lua_State* L, const user& v);
bool from_lua(lua_State* L, user& v);

void reg_user_to_lua(klua::lua& l);
