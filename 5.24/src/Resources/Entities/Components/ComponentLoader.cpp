#include "ComponentLoader.h"

#include <iostream>
#include <string>

#include "FileReader.h"

#include "Component.h"
#include "PositionComponent.h"
#include "SpriteComponent.h"
#include "SpellComponent.h"
#include "MagicComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"
#include "EffectComponent.h"
#include "StatsComponent.h"

#include "Environment.h"
#include "Log.h"

#include "Globals.h"

#define FILE_TEXTURE_ID "itexture_id"

#define FILE_POSITION_COMPONENT "Position"
#define FILE_SPRITE_COMPONENT "Sprite"
#define FILE_SPELL_COMPONENT "Spell"
#define FILE_MAGIC_COMPONENT "Magic"
#define FILE_PLAYER_COMPONENT "Player"
#define FILE_ENEMY_COMPONENT "Enemy"
#define FILE_EFFECT_COMPONENT "Effect"
#define FILE_STATS_COMPONENT "Stats"

#define FILE_POSITION_WIDTH "iwidth"
#define FILE_POSITION_HEIGHT "iheight"
#define FILE_POSITION_SPEED "fspeed"

#define FILE_SPRITE_WIDTH "iwidth"
#define FILE_SPRITE_HEIGHT "iheight"
#define FILE_SPRITE_TIME "isprite_time"

#define FILE_SPELL_NAME "sspell_name"
#define FILE_SPELL_MAX_DIS "fmax_dis"
#define FILE_SPELL_SPEED "fspell_speed"
#define FILE_SPELL_DEATH_TIME "ideath_time"
#define FILE_SPELL_SCRIPT "sspell_script"
#define FILE_SPELL_DAMAGE "ispell_damage"

#define FILE_MAGIC_MAIN_SPELL_ID "imain_spell_id"
#define FILE_MAGIC_CAST_SPEED "icast_speed"

#define FILE_ENEMY_NAME "senemy_name"
#define FILE_ENEMY_SCRIPT "senemy_script"

#define FILE_EFFECT_NAME "seffect_name"
#define FILE_EFFECT_SCRIPT "seffect_script"
#define FILE_EFFECT_RAND_TIME_OFFSET_RANGE "ieffect_rand_time_offset_range"

#define FILE_STATS_MAX_HEALTH "istats_max_health"
#define FILE_STATS_MAX_MANA "istats_max_mana"
#define FILE_STATS_DAMAGE "istats_damage"
#define FILE_STATS_ARMOR "istats_armor"

void load_position(FileReader &file, Entity *entity, PositionComponent *&position) {
	int w = 32, h = 32;
	float speed = 0.0;

	if (file.exists(FILE_POSITION_WIDTH))  w = file.get_int(FILE_POSITION_WIDTH); 
	if (file.exists(FILE_POSITION_HEIGHT))  h = file.get_int(FILE_POSITION_HEIGHT); 
	if (file.exists(FILE_POSITION_SPEED))  speed = file.get_float(FILE_POSITION_SPEED); 

	position = new PositionComponent(entity, 0.0, 0.0, w, h, speed);
}

void load_sprite(FileReader &file, Entity *entity, SpriteComponent *&sprite) {
	int w = 0, h = 0, time = -1;

	if (file.exists(FILE_SPRITE_WIDTH))  w = file.get_int(FILE_SPRITE_WIDTH); 
	if (file.exists(FILE_SPRITE_HEIGHT))  h = file.get_int(FILE_SPRITE_HEIGHT); 
	if (file.exists(FILE_SPRITE_TIME))  time = file.get_int(FILE_SPRITE_TIME); 

	sprite = new SpriteComponent(entity, w, h, time);
}

void load_spell(FileReader &file, Entity *entity, SpellComponent *&spell) {
	float max_dis = 0;
	float speed = 0;
	int death_time = 0;
	std::string script = " ";
	std::string name = " ";
	int damage = 0;

	if (file.exists(FILE_SPELL_NAME)) name = file.get_string(FILE_SPELL_NAME);
	if (file.exists(FILE_SPELL_MAX_DIS)) max_dis = file.get_float(FILE_SPELL_MAX_DIS);
	if (file.exists(FILE_SPELL_SPEED)) speed = file.get_float(FILE_SPELL_SPEED);
	if (file.exists(FILE_SPELL_DEATH_TIME)) death_time = file.get_int(FILE_SPELL_DEATH_TIME);
	if (file.exists(FILE_SPELL_SCRIPT)) script = file.get_string(FILE_SPELL_SCRIPT);
	if (file.exists(FILE_SPELL_DAMAGE)) damage = file.get_int(FILE_SPELL_DAMAGE);

	spell = new SpellComponent(entity, name, max_dis, speed, death_time, script, damage);
}

void load_magic(FileReader &file, Entity *entity, MagicComponent *&magic) {
	int main_spell_id = 0;
	int cast_speed = 0;

	if (file.exists(FILE_MAGIC_MAIN_SPELL_ID)) main_spell_id = file.get_int(FILE_MAGIC_MAIN_SPELL_ID);
	if (file.exists(FILE_MAGIC_CAST_SPEED)) cast_speed = file.get_int(FILE_MAGIC_CAST_SPEED);

	magic = new MagicComponent(entity, main_spell_id, cast_speed);
}

void load_player(FileReader &file, Entity *entity, PlayerComponent *&player) {
	player = new PlayerComponent(entity);
}

void load_enemy(FileReader &file, Entity *entity, EnemyComponent *&enemy) {
	std::string name = " ";
	std::string script = " ";

	if (file.exists(FILE_ENEMY_NAME)) name = file.get_string(FILE_ENEMY_NAME);
	if (file.exists(FILE_ENEMY_SCRIPT)) script = file.get_string(FILE_ENEMY_SCRIPT);

	enemy = new EnemyComponent(entity, name, script);
}

void load_effect(FileReader &file, Entity *entity, EffectComponent *&effect) {
	std::string name = " ";
	std::string script = " ";
	int rand_time_offset_range = 0;

	if (file.exists(FILE_EFFECT_NAME)) name = file.get_string(FILE_EFFECT_NAME);
	if (file.exists(FILE_EFFECT_SCRIPT)) script = file.get_string(FILE_EFFECT_SCRIPT);
	if (file.exists(FILE_EFFECT_RAND_TIME_OFFSET_RANGE)) rand_time_offset_range = file.get_int(FILE_EFFECT_RAND_TIME_OFFSET_RANGE);

	effect = new EffectComponent(entity, name, script, rand_time_offset_range);
}

void load_stats(FileReader &file, Entity *entity, StatsComponent *&stats) {
	int max_health = 0;
	int max_mana = 0;
	int damage = 0;
	int armor = 0;

	if (file.exists(FILE_STATS_MAX_HEALTH)) max_health = file.get_int(FILE_STATS_MAX_HEALTH);
	if (file.exists(FILE_STATS_MAX_MANA)) max_mana = file.get_int(FILE_STATS_MAX_MANA);
	if (file.exists(FILE_STATS_DAMAGE)) damage = file.get_int(FILE_STATS_DAMAGE);
	if (file.exists(FILE_STATS_ARMOR)) armor = file.get_int(FILE_STATS_ARMOR);

	stats = new StatsComponent(entity, max_health, max_mana, damage, armor);
}

bool load_components(Entity *entity) {
	/*
	Environment::get().get_log()->print(
		"Loading Entity with type: \n "
		+ entity->get_type()
		+ "\n type_id: "
		+ std::to_string(entity->get_type_id())
	);
	*/

	std::string locate_file_path = ENTITY_FOLDER + STYPE(entity->get_type()) + "/" + STYPE(entity->get_type()) + ".txt";
	FileReader locate_file(locate_file_path.c_str());
	std::string entity_type_id = std::to_string(entity->get_type_id());

	if (!locate_file.exists(entity_type_id)) {
		Environment::get().get_log()->print(
			"No entity with type_id of: \""
			+ std::to_string(entity->get_type_id())
			+ "\" exits."
		);
		return false;
	}

	FileReader file(locate_file.get_string(entity_type_id).c_str());

	int numComponents = 0;
	entity->clear();

	if (file.exists(FILE_TEXTURE_ID)) entity->set_texture_id(file.get_int(FILE_TEXTURE_ID));

	//Environment::get().get_log()->print("Loading Components: ", "\n", false);

	if (file.exists(FILE_POSITION_COMPONENT)) {
	//	Environment::get().get_log()->print("Position ", "\n", false);
		PositionComponent *position = nullptr;
		load_position(file, entity, position);
		entity->add_component(position);
		numComponents++;
	}

	if (file.exists(FILE_SPRITE_COMPONENT)) {
	//	Environment::get().get_log()->print("Sprite ", "\n", false);
		SpriteComponent *sprite = nullptr;
		load_sprite(file, entity, sprite);
		entity->add_component(sprite);
		numComponents++;
	}

	if (file.exists(FILE_SPELL_COMPONENT)) {
	//	Environment::get().get_log()->print("Spell ", "\n", false);
		SpellComponent *spell = nullptr;
		load_spell(file, entity, spell);
		entity->add_component(spell);
		numComponents++;
	}

	if (file.exists(FILE_MAGIC_COMPONENT)) {
	//	Environment::get().get_log()->print("Magic ", "\n", false);
		MagicComponent *magic = nullptr;
		load_magic(file, entity, magic);
		entity->add_component(magic);
		numComponents++;
	}

	if (file.exists(FILE_PLAYER_COMPONENT)) {
		PlayerComponent *player = nullptr;
		load_player(file, entity, player);
		entity->add_component(player);
		numComponents++;
	}

	if (file.exists(FILE_ENEMY_COMPONENT)) {
		EnemyComponent *enemy = nullptr;
		load_enemy(file, entity, enemy);
		entity->add_component(enemy);
		numComponents++;
	}

	if (file.exists(FILE_EFFECT_COMPONENT)) {
		EffectComponent *effect = nullptr;
		load_effect(file, entity, effect);
		entity->add_component(effect);
		numComponents++;
	}

	if (file.exists(FILE_STATS_COMPONENT)) {
		StatsComponent *stats = nullptr;
		load_stats(file, entity, stats);
		entity->add_component(stats);
		numComponents++;
	}

	//Environment::get().get_log()->print("- " + std::to_string(numComponents) + " Component(s).", "\n", false);

	return true;
}