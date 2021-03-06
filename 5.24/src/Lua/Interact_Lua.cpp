#include "Interact_Lua.h"

#include "InteractComponent.h"

#include "_Lua.h"
#include "ResourceManager.h"

// not implemented
static InteractComponent *interact_new(lua_State *L) {
	return nullptr;
}

// Interact:find_gate(int button_channel, string object_name)
// returns Gate Entity if found
// or nil if not found
static int interact_find_gate(lua_State *L) {
	std::string data = luaL_checkstring(L, -2);
	std::string name = luaL_checkstring(L, -1);
	
	const auto &objects = Environment::get().get_resource_manager()->get_entities(TYPE_OBJECT);

	for (auto it = objects->begin(); it != objects->end(); ++it) {
		if (InteractComponent *interact = GetInteract(it->second)) {
			if (interact->data == data && interact->name == name) {
				luaW_push<Entity>(L, it->second);
				return 1;
			}
		}
	}

	lua_pushnil(L);
	return 1;
}

// Interact:get_data()
static int interact_get_data(lua_State *L) {
	InteractComponent *interact = luaW_check<InteractComponent>(L, -1);
	lua_pushstring(L, interact->data.c_str());
	return 1;
}

// Entity Interact:get_entity()
static int interact_get_entity(lua_State *L) {
	InteractComponent *interact = luaW_check<InteractComponent>(L, -1);
	luaW_push<Entity>(L, interact->entity);
	return 1;
}

static luaL_Reg interact_table[] = {
	{NULL, NULL}
};

static luaL_Reg interact_metatable[] = {
	// func

	// set

	// get
	{"get_data", interact_get_data},
	{"get_entity", interact_get_entity},

	{NULL, NULL}
};

void lua_init_interact(Lua *lua, lua_State *L) {
	luaW_register<InteractComponent>(L, "Interact", interact_table, interact_metatable, interact_new);
	lua->register_global("interact_find_gate", interact_find_gate);
}