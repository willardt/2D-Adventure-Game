#include "Entity_Lua.h"

#include "Entity.h"
#include "PositionComponent.h"
#include "SpriteComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"
#include "SpellComponent.h"
#include "MagicComponent.h"
#include "EffectComponent.h"
#include "CombatComponent.h"
#include "NPCComponent.h"

#include "Environment.h"
#include "ResourceManager.h"
#include "Log.h"

static Entity *entity_new(lua_State *L) {
	int type = (int)luaL_checknumber(L, -2);
	int id = (int)luaL_checknumber(L, -1);
	return new Entity(type, id);
}

// void Entity:destroy()
static int entity_destroy(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	entity->destroy();
	return 0;
}

// PositionComponent Entity:get_position()
static int entity_get_position(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	PositionComponent *position = GetPosition(entity);
	if (position)
		luaW_push<PositionComponent>(L, position);
	else
		lua_pushnil(L);
	return 1;
}

// SpriteComponent Entity:get_sprite()
static int entity_get_sprite(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	SpriteComponent *sprite = GetSprite(entity);
	if (sprite)
		luaW_push<SpriteComponent>(L, sprite);
	else
		lua_pushnil(L);
	return 1;
}

// PlayerComponent Entity:get_player()
static int entity_get_player(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	PlayerComponent *player = GetPlayer(entity);
	if (player)
		luaW_push<PlayerComponent>(L, player);
	else
		lua_pushnil(L);
	return 1;
}

// EnemyComponent Entity:get_enemy()
static int entity_get_enemy(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	EnemyComponent *enemy = GetEnemy(entity);
	if (enemy)
		luaW_push<EnemyComponent>(L, enemy);
	else
		lua_pushnil(L);
	return 1;
}

// SpellComponent Entity:get_spell()
static int entity_get_spell(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	SpellComponent *spell = GetSpell(entity);
	if (spell)
		luaW_push<SpellComponent>(L, spell);
	else
		lua_pushnil(L);
	return 1;
}

// MagicComponent Entity:get_magic()
static int entity_get_magic(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	MagicComponent *magic = GetMagic(entity);
	if (magic)
		luaW_push<MagicComponent>(L, magic);
	else
		lua_pushnil(L);
	return 1;
}

// EffectComponent Entity:get_effect()
static int entity_get_effect(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	EffectComponent *effect = GetEffect(entity);
	if (effect)
		luaW_push<EffectComponent>(L, effect);
	else
		lua_pushnil(L);
	return 1;
}

// CombatComponent Entity:get_combat()
static int entity_get_combat(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	CombatComponent *combat = GetCombat(entity);
	if (combat)
		luaW_push<CombatComponent>(L, combat);
	else
		lua_pushnil(L);
	return 1;
}

// NPCComponent Entity:get_npc()
static int entity_get_npc(lua_State *L) {
	Entity *entity = luaW_check<Entity>(L, -1);
	NPCComponent *npc = GetNPC(entity);
	if (npc)
		luaW_push<NPCComponent>(L, npc);
	else
		lua_pushnil(L);
	return 1;
}



static luaL_Reg entity_table[] = {
	{NULL, NULL}
};

static luaL_Reg entity_metatable[] = {
	// func
	{"destroy", entity_destroy},


	// set

	// get
	{"get_position", entity_get_position},
	{"get_sprite", entity_get_sprite},
	{"get_player", entity_get_player},
	{"get_enemy", entity_get_enemy},
	{"get_spell", entity_get_spell},
	{"get_magic", entity_get_magic},
	{"get_effect", entity_get_effect},
	{"get_combat", entity_get_combat},
	{"get_npc", entity_get_npc},

	{NULL, NULL}
};

// void create_entity(int type, int type_id, float x, float y)
static int create_entity(lua_State *L) {
	int type = (int)luaL_checknumber(L, -4);
	int type_id = (int)luaL_checknumber(L, -3);
	float x = (float)luaL_checknumber(L, -2);
	float y = (float)luaL_checknumber(L, -1);

	Environment::get().get_resource_manager()->create_entity(type, type_id, x, y);

	return 0;
}

void lua_init_entity(Lua *lua, lua_State *L) {
	luaW_register<Entity>(L, "Entity", entity_table, entity_metatable, entity_new);
	lua->register_global("create_entity", create_entity);
}