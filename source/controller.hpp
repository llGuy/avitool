#pragma once

#include <utility>
#include <stdint.h>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

enum class command_type_t : uint32_t {
    LOAD_FILE,
    GOTO_VIDEO_FRAME,
    GOTO_VIDEO_TIME,
    BEGIN_RECORD,
    ADD_RECORD_POINT,
    END_RECORD,
    MAKE_AXES,
    INVALID
};

void init_controller();

/*
  If the command string is syntactically correct,
  push the command
 */
void submit_cmdstr(const char *cmdstr);
void execute_commands();
const char *cmd_type_to_str(command_type_t type);

extern struct lua_State *g_lua_state;

// --- Sorry about this stuff ---
template <typename T>
T get_from_lua_stack(int32_t pos);

template <typename T>
T get_return_value() {
    lua_getglobal(g_lua_state, "ret");
    return get_from_lua_stack<T>(-1);
}

template <typename T>
void push_to_lua_stack(T value);

template <typename Ret, typename ...T, int32_t ...I>
int32_t call_cmd_function_impl(
    Ret (* proc)(T ...),
    std::integer_sequence<int32_t, I...> stack_indices) {
    constexpr int32_t arg_count = sizeof...(T);
    Ret ret = proc(get_from_lua_stack<T>(-arg_count + I)...);
    push_to_lua_stack(ret);

    return 1;
}

template <typename Ret, typename ...T>
int32_t call_cmd_function(Ret (* proc)(T ...)) {
    return call_cmd_function_impl<Ret, T...>(
        proc,
        std::make_integer_sequence<int32_t, sizeof...(T)>());
}

#define DECLARE_CMD_PROC(return_type, name, ...)        \
    return_type name(__VA_ARGS__); \
    inline int32_t name##_impl(struct lua_State *) { return call_cmd_function(name); }

#define BIND_CMD_TO_PROC(type, func)                    \
    lua_pushcfunction(g_lua_state, func##_impl);        \
    lua_setglobal(g_lua_state, cmd_type_to_str(type))
