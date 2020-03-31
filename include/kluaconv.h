#pragma once
#include <cstdint>
#include <string>
#include <initializer_list>
#include <vector>
#include <list>
#include <array>
#include <valarray>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include "k/lua.h"

namespace klua {

inline void to_lua(lua_State* L, bool v) { lua_pushboolean(L, (v ? 1 : 0)); }

inline void to_lua(lua_State* L, const char* s) { lua_pushstring(L, s); }

inline void to_lua(lua_State* L, const std::string& s) { lua_pushlstring(L, s.c_str(), s.size()); }

template <typename T, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
inline void to_lua(lua_State* L, T v)
{
    to_lua(L, static_cast<typename std::underlying_type<T>::type>(v));
}

template <typename T,
    typename std::enable_if<!std::is_same<typename std::remove_cv<T>::type, bool>::value
            && std::is_integral<T>::value,
        int>::type
    = 0>
inline void to_lua(lua_State* L, T v)
{
    lua_pushinteger(L, static_cast<lua_Integer>(v));
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
inline void to_lua(lua_State* L, T v)
{
    lua_pushnumber(L, static_cast<lua_Number>(v));
}

template <typename T>
inline void to_lua(lua_State* L, const std::initializer_list<T>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename T, typename A>
inline void to_lua(lua_State* L, const std::vector<T, A>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename T, typename A>
inline void to_lua(lua_State* L, const std::list<T, A>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename T, std::size_t N>
inline void to_lua(lua_State* L, const std::array<T, N>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename T>
inline void to_lua(lua_State* L, const std::valarray<T>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename T, std::size_t N>
inline void to_lua(lua_State* L, const T (&v)[N])
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename K, typename P, typename A>
inline void to_lua(lua_State* L, const std::set<K, P, A>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename K, typename H, typename E, typename A>
inline void to_lua(lua_State* L, const std::unordered_set<K, H, E, A>& v)
{
    lua_newtable(L);
    int i = 1;
    for (const auto& x : v) {
        to_lua(L, x);
        lua_rawseti(L, -2, i);
        ++i;
    }
}

template <typename K, typename V, typename P, typename A>
inline void to_lua(lua_State* L, const std::map<K, V, P, A>& v)
{
    lua_newtable(L);
    for (const auto& kv : v) {
        to_lua(L, kv.first);
        to_lua(L, kv.second);
        lua_rawset(L, -3);
    }
}

template <typename K, typename V, typename H, typename E, typename A>
inline void to_lua(lua_State* L, const std::unordered_map<K, V, H, E, A>& v)
{
    lua_newtable(L);
    for (const auto& kv : v) {
        to_lua(L, kv.first);
        to_lua(L, kv.second);
        lua_rawset(L, -3);
    }
}

inline bool from_lua(lua_State* L, bool& v)
{
    if (LUA_TBOOLEAN != lua_type(L, -1))
        return false;
    v = (0 != lua_toboolean(L, -1));
    lua_pop(L, 1);
    return true;
}

inline bool from_lua(lua_State* L, std::string& v)
{
    if (LUA_TSTRING != lua_type(L, -1))
        return false;
    size_t len;
    const auto* str = lua_tolstring(L, -1, &len);
    if (len > 0)
        v.assign(str, str + len);
    else
        v.clear();
    lua_pop(L, 1);
    return true;
}

template <typename T, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
inline bool from_lua(lua_State* L, T& v)
{
    typename std::underlying_type<T>::type x;
    if (!from_lua(L, x))
        return false;
    v = static_cast<T>(x);
    lua_pop(L, 1);
    return true;
}

template <typename T,
    typename std::enable_if<!std::is_same<typename std::remove_cv<T>::type, bool>::value
            && std::is_integral<T>::value,
        int>::type
    = 0>
inline bool from_lua(lua_State* L, T& v)
{
    if (LUA_TNUMBER != lua_type(L, -1))
        return false;
    v = static_cast<T>(lua_tointeger(L, -1));
    lua_pop(L, 1);
    return true;
}

template <typename T, typename std::enable_if<std::is_floating_point<T>::value, int>::type = 0>
inline bool from_lua(lua_State* L, T& v)
{
    if (LUA_TNUMBER != lua_type(L, -1))
        return false;
    v = static_cast<T>(lua_tonumber(L, -1));
    lua_pop(L, 1);
    return true;
}

template <typename T, typename A>
inline bool from_lua(lua_State* L, std::vector<T, A>& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    lua_pushnil(L);
    while (0 != lua_next(L, -2)) {
        T x;
        if (!from_lua(L, x)) {
            lua_pop(L, 2);
            return false;
        }
        v.push_back(x);
    }
    return true;
}

template <typename T, typename A>
inline bool from_lua(lua_State* L, std::list<T, A>& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    lua_pushnil(L);
    while (0 != lua_next(L, -2)) {
        T x;
        if (!from_lua(L, x)) {
            lua_pop(L, 2);
            return false;
        }
        v.push_back(x);
    }
    return true;
}

template <typename K, typename P, typename A>
inline bool from_lua(lua_State* L, std::set<K, P, A>& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    lua_pushnil(L);
    while (0 != lua_next(L, -2)) {
        K x;
        if (!from_lua(L, x)) {
            lua_pop(L, 2);
            return false;
        }
        v.push_back(x);
    }
    return true;
}

template <typename K, typename H, typename E, typename A>
inline bool from_lua(lua_State* L, std::unordered_set<K, H, E, A>& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    lua_pushnil(L);
    while (0 != lua_next(L, -2)) {
        K x;
        if (!from_lua(L, x)) {
            lua_pop(L, 2);
            return false;
        }
        v.push_back(x);
    }
    return true;
}

template <typename K, typename V, typename P, typename A>
inline bool from_lua(lua_State* L, std::map<K, V, P, A>& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    lua_pushnil(L);
    while (0 != lua_next(L, -2)) {
        lua_pushvalue(L, -2);
        K x;
        if (!from_lua(L, x)) {
            lua_pop(L, 3);
            return false;
        }
        V y;
        if (!from_lua(L, y)) {
            lua_pop(L, 2);
            return false;
        }
        v.insert(std::make_pair(x, y));
    }
    return true;
}

template <typename K, typename V, typename H, typename E, typename A>
inline bool from_lua(lua_State* L, std::unordered_map<K, V, H, E, A>& v)
{
    if (LUA_TTABLE != lua_type(L, -1))
        return false;
    lua_pushnil(L);
    while (0 != lua_next(L, -2)) {
        lua_pushvalue(L, -2);
        K x;
        if (!from_lua(L, x)) {
            lua_pop(L, 3);
            return false;
        }
        V y;
        if (!from_lua(L, y)) {
            lua_pop(L, 2);
            return false;
        }
        v.insert(std::make_pair(x, y));
    }
    return true;
}

} // namespace klua
