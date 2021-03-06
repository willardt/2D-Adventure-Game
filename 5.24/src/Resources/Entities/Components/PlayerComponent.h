#include <vector>
#include <array>
#include <string>

#include "Component.h"
#include "Entity.h"

#include "Inventory.h"
#include "Quest.h"

#ifndef PLAYER_COMPONENT_H
#define PLAYER_COMPONENT_H

#define MAX_ITEMS 100
#define QUEST_LOG_CAPACITY 50

#define FIRE_ONE_ID 0
#define FIRE_TWO_ID 0
#define FIRE_THREE_ID 0
#define FIRE_FOUR_ID 0

#define WATER_ONE_ID 5
#define WATER_TWO_ID 0
#define WATER_THREE_ID 0
#define WATER_FOUR_ID 0

#define EARTH_ONE_ID 6
#define EARTH_TWO_ID 0
#define EARTH_THREE_ID 0
#define EARTH_FOUR_ID 0

#define AIR_ONE_ID 7
#define AIR_TWO_ID 0
#define AIR_THREE_ID 0
#define AIR_FOUR_ID 0

struct PlayerComponent : public Component {
	PlayerComponent(Entity *entity_, std::string name);
	PlayerComponent(Entity *new_entity, const PlayerComponent &rhs);
	PlayerComponent *copy(Entity *new_entity) const;

	~PlayerComponent();

	void update();

	const int get_type() const { return COMPONENT_PLAYER; }

	bool is_collision();

	void equip_item(int index);
	void unequip_item(int index);
	void equip_item_stats(ItemComponent *item);
	void unequip_item_stats(ItemComponent *item);

	void level_up();
	void add_exp(int amount);

	void warp(int map_id, SDL_Rect warp_rect);

	int level, exp, exp_to_level;
	int gold;
	std::string name;
	std::vector<Entity *> items;
	std::array<Entity *, TOTAL_SLOTS> equipped_items = { nullptr };
	std::array<Quest *, QUEST_LOG_CAPACITY> quest_log = { nullptr };
	std::array<bool, TOTAL_PLAYER_SPELLS> known_spells = { 0 };

	Inventory inventory;

	bool debug_collision;
};

#endif