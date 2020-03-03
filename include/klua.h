#pragma once
#include "kluautil.h"


namespace klua
{
    class lua
    {
    public:
        lua() noexcept;
        ~lua() noexcept;

        bool load_stdlibs();

        bool add_path(const std::string& path);
        bool add_cpath(const std::string& cpath);

        bool requiref(const std::string& modname, lua_CFunction openf, bool global);

        bool dofile(const std::string& filename);
        bool dostring(const std::string& str);

        bool getglobal(const std::string& name);

        bool call(int narg, int nret);

        template <typename V>
        void to_lua(V&& v) const
        {
            using ::klua::to_lua;
            to_lua(_lua, std::forward<V>(v));
        }

        template <typename V>
        bool from_lua(V& v) const
        {
            using ::klua::from_lua;
            return from_lua(_lua, v);
        }

        bool call(const std::string& name)
        {
            if (!getglobal(name))
                return false;
            return call(0, 0);
        }

        template <typename P0>
        bool call1p(const std::string& name, P0&& p0)
        {
            if (!getglobal(name))
                return false;
            to_lua(p0);
            return call(1, 0);
        }

        template <typename P0, typename P1>
        bool call2p(const std::string& name, P0&& p0, P1&& p1)
        {
            if (!getglobal(name))
                return false;
            to_lua(p0);
            to_lua(p1);
            return call(2, 0);
        }

        template <typename P0, typename R0>
        bool call1p(const std::string& name, P0&& p0, R0& r0)
        {
            if (!getglobal(name))
                return false;
            to_lua(p0);
            if (!call(1, 1))
                return false;
            if (from_lua(r0))
                return true;
            _err = "from_lua failed";
            pop(1);
            return false;
        }

        void pop(int n) const noexcept
        {
            lua_pop(_lua, n);
        }
        int gettop() const noexcept
        {
            return lua_gettop(_lua);
        }

        const std::string& get_err() const noexcept
        {
            return _err;
        }
        void set_err(const std::string& err)
        {
            _err = err;
        }

        int save() noexcept
        {
            _save = lua_gettop(_lua); return _save;
        }
        int load() noexcept
        {
            lua_settop(_lua, _save); return _save;
        }

        operator lua_State*() const noexcept
        {
            return _lua;
        }

    private:
        bool check(int status);

    private:
        lua_State* _lua;
        std::string _err;
        int _save;
    };
}
