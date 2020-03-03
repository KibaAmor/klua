#pragma once
#include <klua.h>
#include <atomic>


class usermgr
{
    usermgr() = default;
public:
    static usermgr& instance()
    {
        static usermgr um;
        return um;
    }

    void inc_user_count() noexcept { _user_count.fetch_add(1, std::memory_order_relaxed); }
    void dec_user_count() noexcept { _user_count.fetch_sub(1, std::memory_order_relaxed); }
    int64_t get_user_count() const noexcept { return _user_count.load(std::memory_order_consume); }

private:
    std::atomic<int64_t> _user_count = { 0 };
};


void reg_usermgr_to_lua(const klua::lua& l, usermgr& um);
