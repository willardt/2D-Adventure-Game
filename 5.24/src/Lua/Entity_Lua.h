#include "luawrapper.hpp"

#ifndef ENTITY_LUA_H
#define ENTITY_LUA_H

class Lua;

void lua_init_entity(Lua *lua, lua_State *L);

#endif