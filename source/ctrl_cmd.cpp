#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "ctrl_cmd.hpp"
#include "va_analyser.hpp"

namespace ctrl {

lua_State *g_lua_state = NULL;
static const char *cmd_names[(uint32_t)command_type_t::INVALID];

void init_controller() {
    g_lua_state = luaL_newstate();
    luaL_openlibs(g_lua_state);

    cmd_names[(uint32_t)command_type_t::LOAD_FILE] = "load_file";
    cmd_names[(uint32_t)command_type_t::GOTO_VIDEO_FRAME] = "goto_video_frame";
    cmd_names[(uint32_t)command_type_t::GOTO_VIDEO_TIME] = "goto_video_time";
    cmd_names[(uint32_t)command_type_t::BEGIN_RECORD] = "begin_record";
    cmd_names[(uint32_t)command_type_t::ADD_RECORD_POINT] = "add_record_point";
    cmd_names[(uint32_t)command_type_t::END_RECORD] = "end_record";
    cmd_names[(uint32_t)command_type_t::MAKE_AXES] = "make_axes";
 
    BIND_CMD_TO_PROC(command_type_t::LOAD_FILE, cmd_load_file);
    BIND_CMD_TO_PROC(command_type_t::GOTO_VIDEO_FRAME, cmd_goto_video_frame);
    BIND_CMD_TO_PROC(command_type_t::GOTO_VIDEO_TIME, cmd_goto_video_time);
    BIND_CMD_TO_PROC(command_type_t::BEGIN_RECORD, cmd_begin_record);
    BIND_CMD_TO_PROC(command_type_t::ADD_RECORD_POINT, cmd_add_record_point);
    BIND_CMD_TO_PROC(command_type_t::END_RECORD, cmd_end_record);
    BIND_CMD_TO_PROC(command_type_t::MAKE_AXES, cmd_make_axes);
}

void submit_cmdstr(const char *cmdstr) {
    static char actual_cmd[100] = {};
    sprintf(actual_cmd, "ret = %s", cmdstr);

    int32_t error = luaL_loadbuffer(g_lua_state, actual_cmd, strlen(actual_cmd), actual_cmd) ||
        lua_pcall(g_lua_state, 0, 0, 0);

    if (error) {
        const char *error = lua_tostring(g_lua_state, -1);
        lua_pop(g_lua_state, 1);
    }
}

const char *cmd_type_to_str(command_type_t type) {
    assert((uint32_t)type < (uint32_t)command_type_t::INVALID);
    return cmd_names[(uint32_t)type];
}

template <>
const char *get_from_lua_stack<const char *>(int32_t pos){
    return lua_tostring(g_lua_state, pos);
}

template <>
int32_t get_from_lua_stack<int32_t>(int32_t pos){
    return lua_tonumber(g_lua_state, pos);
}

template <>
void push_to_lua_stack<const char *>(const char *value){
    return lua_pushstring(g_lua_state, value);
}

template <>
void push_to_lua_stack<int32_t>(int32_t value){
    return lua_pushnumber(g_lua_state, value);
}

}
