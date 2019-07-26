#include "SpellComponent.h"

#include "Entity.h"
#include "PositionComponent.h"
#include "SpriteComponent.h"
#include "StatsComponent.h"

#include "Environment.h"
#include "Clock.h"
#include "_Lua.h"
#include "ResourceManager.h"

#include "luawrapper.hpp"

#include <iostream>

SpellComponent::SpellComponent(Entity *entity_, std::string name_, float max_dis_, float speed_, int death_time_, std::string script_, int damage_) :
	Component			( entity_ ),
	name				( name_ ),
	caster				( nullptr ),
	dis					( 0 ),
	max_dis				( max_dis_ ),
	speed				( speed_ ),
	dx					( 0 ),
	dy					( 0 ),
	dead				( false ),
	destroy				( false ),
	death_timer			( death_time_ ),
	script				( script_ ),
	damage				( damage_ )
{
	Environment::get().get_lua()->load_script(script);
}

SpellComponent::SpellComponent(Entity *new_entity, const SpellComponent &rhs) :
	Component			( new_entity ),
	name				( rhs.name ),
	caster				( rhs.caster ),
	dis					( rhs.dis ),
	max_dis				( rhs.max_dis ),
	speed				( rhs.speed ),
	dx					( rhs.dx ),
	dy					( rhs.dy ),
	dead				( rhs.dead ),
	destroy				( rhs.destroy ),
	death_timer			( rhs.death_timer ),
	script				( rhs.script ),
	damage				( rhs.damage )
{}

SpellComponent *SpellComponent::copy(Entity *new_entity) const {
	return new SpellComponent(new_entity, *this);
}

void SpellComponent::update() {
	if (!dead) {
		lua_State *L = Environment::get().get_lua()->get_state();
		lua_getglobal(L, name.c_str());
		lua_getfield(L, -1, "update");
		lua_remove(L, -2);
		luaW_push<SpellComponent>(L, this);
		lua_pcall(L, 1, 0, 0);
	}
	else {
		if (death_timer.update()) {
			entity->destroy();
		}
	}
}

void SpellComponent::cast() {
	lua_State *L = Environment::get().get_lua()->get_state();
	lua_getglobal(L, name.c_str());
	lua_getfield(L, -1, "cast");
	lua_remove(L, -2);
	luaW_push<SpellComponent>(L, this);
	lua_pcall(L, 1, 0, 0);
}

void SpellComponent::move(float x, float y) {
	float xdis = float(x * Environment::get().get_clock()->get_time());
	float ydis = float(y * Environment::get().get_clock()->get_time());

	dis += abs(xdis) + abs(ydis);

	if (PositionComponent *position = GetPosition(entity)) {
		position->pos_x += xdis;
		position->pos_y += ydis;
		position->rect.x = (int)position->pos_x;
		position->rect.y = (int)position->pos_y;
		if (is_collision()) {
			death();
		}
	}
}

bool SpellComponent::is_collision() {
	PositionComponent *position = GetPosition(entity);
	if (!position)
		return false;

	const auto entity_vec = Environment::get().get_resource_manager()->get_map()->get_entity_grid()->get_cells(position->rect);

	for (auto &vec : entity_vec) {
		for (auto &e : *vec) {
			const int &type = e->get_type();
			if (type == TYPE_OBJECT ||
				type == TYPE_ENEMY ||
				type == TYPE_PLAYER) {
				if (collision(position->rect, GetPosition(e)->rect) &&
					e != caster) {

					if (StatsComponent *stats = GetStats(e)) {
						stats->apply_damage(calc_damage());
					}

					return true;
				}
			}
		}
	}

	return false;
}

void SpellComponent::death() {
	dead = true;
	death_timer.reset();
	if (SpriteComponent *sprite = GetSprite(entity)) {
		Sprite *sprite_info = Environment::get().get_resource_manager()->get_sprite_info(entity);
		sprite->pos.x = sprite_info->get_frame(sprite_info->end).x;
		sprite->pos.y = sprite_info->get_frame(sprite_info->end).y;
		sprite->time = -1;
	}
}

int SpellComponent::calc_damage() {
	if (StatsComponent *stats = GetStats(entity)) {
		return stats->damage + damage;
	}

	return damage;
}