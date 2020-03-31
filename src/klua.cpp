#include "klua.h"
#include "k/lualib.h"
#include <cstdio>

namespace {

const char* klua_lib = R"(
klua = {}

function klua.dump(value, desc, nesting)
    if type(nesting) ~= "number" then nesting = 3 end

    local lookupTable = {}
    local result = {}

    local function _v(v)
        if type(v) == "string" then
            v = "\"" .. v .. "\""
        end
        return tostring(v)
    end

    -- local traceback = string.split(debug.traceback("", 2), "\n")
    -- print("dump from: " .. string.trim(traceback[3]))

    local function _dump(value, desc, indent, nest, keylen)
        desc = desc or "<var>"
        spc = ""
        if type(keylen) == "number" then
            spc = string.rep(" ", keylen - string.len(_v(desc)))
        end
        if type(value) ~= "table" then
            result[#result +1 ] = string.format("%s%s%s = %s", indent, _v(desc), spc, _v(value))
        elseif lookupTable[value] then
            result[#result +1 ] = string.format("%s%s%s = *REF*", indent, desc, spc)
        else
            lookupTable[value] = true
            if nest > nesting then
                result[#result +1 ] = string.format("%s%s = *MAX NESTING*", indent, desc)
            else
                result[#result +1 ] = string.format("%s%s = {", indent, _v(desc))
                local indent2 = indent.."    "
                local keys = {}
                local keylen = 0
                local values = {}
                for k, v in pairs(value) do
                    keys[#keys + 1] = k
                    local vk = _v(k)
                    local vkl = string.len(vk)
                    if vkl > keylen then keylen = vkl end
                    values[k] = v
                end
                table.sort(keys, function(a, b)
                    if type(a) == "number" and type(b) == "number" then
                        return a < b
                    else
                        return tostring(a) < tostring(b)
                    end
                end)
                for i, k in ipairs(keys) do
                    _dump(values[k], k, indent2, nest + 1, keylen)
                end
                result[#result +1] = string.format("%s}", indent)
            end
        end
    end
    _dump(value, desc, "- ", 1)

    for i, line in ipairs(result) do
        print(line)
    end
end
)";

} // namespace

namespace klua {

lua::lua() noexcept
    : _save(0)
{
    _lua = luaL_newstate();
    luaL_checkversion(_lua);
}

lua::~lua() noexcept
{
    lua_close(_lua);
    _lua = nullptr;
}

bool lua::load_stdlibs()
{
    luaL_openlibs(_lua);
    return dostring(klua_lib);
}

bool lua::add_path(const std::string& path)
{
    return dostring("package.path = package.path .. ';" + path + "/?.lua'");
}

bool lua::add_cpath(const std::string& cpath)
{
#ifdef _WIN32
    return dostring("package.cpath = package.cpath .. ';" + cpath + "/?.dll'");
#else
    return dostring("package.cpath = package.cpath .. ';" + cpath + "/?.so'");
#endif
}

bool lua::requiref(const std::string& modname, lua_CFunction openf, bool global)
{
    luaL_requiref(_lua, modname.c_str(), openf, (global ? 1 : 0));
    const int type = lua_type(_lua, -1);
    pop(1);
    if (LUA_TNIL != type)
        return true;
    _err = "requiref failed for module: " + modname;
    return false;
}

bool lua::dofile(const std::string& filename)
{
    return check(luaL_dofile(_lua, filename.c_str()));
}

bool lua::dostring(const std::string& str)
{
    return check(luaL_dostring(_lua, str.c_str()));
}

bool lua::getglobal(const std::string& name)
{
    auto pos = name.find('.');
    if (std::string::npos != pos) {
        std::string mod = name.substr(0, pos);
        if (LUA_TTABLE != lua_getglobal(_lua, mod.c_str())) {
            _err = std::string("cannot find module '") + name + "'";
            lua_pop(_lua, 1);
            return false;
        }

        std::string sym = name.substr(pos + 1);
        if (LUA_TNIL != lua_getfield(_lua, -1, sym.c_str())) {
            lua_remove(_lua, -2);
            return true;
        }
        _err = "cannot find symbol '" + sym + "'";
        lua_pop(_lua, 2);
    } else {
        if (LUA_TNIL != lua_getglobal(_lua, name.c_str()))
            return true;
        _err = "cannot find symbol '" + name + "'";
        lua_pop(_lua, 1);
    }
    return false;
}

bool lua::call(int narg, int nret)
{
    return check(lua_pcall(_lua, narg, nret, 0));
}

bool lua::check(int status)
{
    if (LUA_OK == status)
        return true;
    const auto* msg = lua_tostring(_lua, -1);
    if (nullptr != msg)
        _err = msg;
    lua_pop(_lua, 1);
    return false;
}

} // namespace klua
