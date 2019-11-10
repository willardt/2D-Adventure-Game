#include "Commands_Lua.h"

#include "Environment.h"
#include "_Lua.h"
#include "ResourceManager.h"

#include "Component.h"
#include "PlayerComponent.h"

// void teleport(int map_id)
static int teleport(lua_State *L) {
	int map_id = (int)luaL_checknumber(L, -1);
	Environment::get().get_resource_manager()->load_map(map_id, false);
	return 0;
}

// void player_collision()
static int player_collision(lua_State *L) {
	PlayerComponent *player = GetPlayer(Environment::get().get_resource_manager()->get_player());
	player->debug_collision = !player->debug_collision;

	if (player->debug_collision) {
		Environment::get().get_resource_manager()->get_textbox().print("Player Collision Off");
		printf("Player Collision Off");
	}
	else {
		Environment::get().get_resource_manager()->get_textbox().print("Player Collision Off");
		printf("Player Collision On");
	}

	return 0;
}

void lua_init_commands(Lua *lua, lua_State *L) {
	lua->register_global("teleport", teleport);
	lua->register_global("player_collision", player_collision);
}