#include "PlayerLoader.h"

#include <sstream>
#include <fstream>

#include "Environment.h"
#include "ResourceManager.h"
#include "Window.h"

#include "QuestLoader.h"

#include "FileReader.h"

#include "Globals.h"

#include "Entity.h"
#include "PositionComponent.h"
#include "ItemComponent.h"
#include "PlayerComponent.h"
#include "MagicCOmponent.h"

#define FILE_PLAYER_POSITION "position"

#define FILE_PLAYER_INVENTORY "inventory"
#define FILE_PLAYER_EQUIPPED "equipped"
#define FILE_PLAYER_BUFFS "buffs"
#define FILE_PLAYER_QUEST_LOG "quest_log"
#define FILE_PLAYER_SPELLS "spells"

#define FILE_PLAYER_NAME "sname"
#define FILE_PLAYER_LEVEL "ilevel"
#define FILE_PLAYER_EXP "iexp"
#define FILE_PLAYER_HEALTH "ihealth"
#define FILE_PLAYER_MANA "imana"
#define FILE_PLAYER_GOLD "igold"
#define FILE_PLAYER_MAP_ID "imap_id"
#define FILE_PLAYER_MAIN_SPELL "imain_spell"
#define FILE_PLAYER_SECONDARY_SPELL "isecondary_spell"

#define FILE_PLAYER_SEPERATOR "."

void load_position(FileReader &file, Entity *player) {
	PositionComponent *player_position = GetPosition(player);

	float x = 0, y = 0;

	if (file.exists(FILE_PLAYER_POSITION)) {
		std::istringstream position(file.get_string(FILE_PLAYER_POSITION));
		position >> x >> y;
	}

	player_position->set(x, y);

	Environment::get().get_window()->get_camera()->center(player);
}

void load_items(FileReader &file, Entity *player) {
	PlayerComponent *player_component = GetPlayer(player);

	if (file.exists(FILE_PLAYER_INVENTORY)) {
		std::istringstream inventory(file.get_string(FILE_PLAYER_INVENTORY));
		int item_id = 0;
		while (inventory >> item_id) {
			Entity *item = new Entity(TYPE_ITEM, item_id);
			player_component->items.push_back(item);
		}
	}

	if (file.exists(FILE_PLAYER_EQUIPPED)) {
		std::istringstream equipped(file.get_string(FILE_PLAYER_EQUIPPED));
		int item_id = 0;
		int slot = 0;
		while (equipped >> item_id) {
			if (item_id != -1) {
				Entity *item = new Entity(TYPE_ITEM, item_id);
				player_component->equipped_items[slot] = item;
				player_component->equip_item_stats(GetItem(item));
			}
			++slot;
		}
	}
}

void load_buffs(FileReader &file, Entity *player) {
	CombatComponent *stats = GetCombat(player);

	if (file.exists(FILE_PLAYER_BUFFS)) {
		std::istringstream buff_data(file.get_string(FILE_PLAYER_BUFFS));
		Buff_Info buff;
		int clock = 0, time = 0;
		while (buff_data >> clock >> time >> buff.health >> buff.mana >> buff.damage >> buff.armor >> buff.speed >> buff.luck >> buff.icon_item_id) {
			buff.timer.set(time);
			buff.timer.reset();
			buff.timer.offset(clock);
			stats->add_buff(buff, false);
		}
	}
}

void load_info(FileReader &file, Entity *player) {
	PlayerComponent *player_component = GetPlayer(player);
	CombatComponent *stats = GetCombat(player);

	std::string name = "";
	int level = 1, exp = 0;
	int health = 0, mana = 0;
	int gold = 0;

	if (file.exists(FILE_PLAYER_NAME)) name = file.get_string(FILE_PLAYER_NAME);
	if (file.exists(FILE_PLAYER_LEVEL)) level = file.get_int(FILE_PLAYER_LEVEL);
	if (file.exists(FILE_PLAYER_EXP)) exp = file.get_int(FILE_PLAYER_EXP);
	if (file.exists(FILE_PLAYER_HEALTH)) health = file.get_int(FILE_PLAYER_HEALTH);
	if (file.exists(FILE_PLAYER_MANA)) mana = file.get_int(FILE_PLAYER_MANA);
	if (file.exists(FILE_PLAYER_GOLD)) gold = file.get_int(FILE_PLAYER_GOLD);

	player_component->name = name;
	player_component->exp = exp;
	player_component->gold = gold;
	stats->health = health;
	stats->mana = mana;

	player_component->level = 1;
	for (int i = 0; i != level - 1; ++i) {
		player_component->level_up();
	}
}

void load_quest_log(FileReader &file, Entity *player) {
	PlayerComponent *player_component = GetPlayer(player);
	
	if (!file.exists(FILE_PLAYER_QUEST_LOG))
		return;

	std::stringstream stream(file.get_string(FILE_PLAYER_QUEST_LOG));
	std::string quest_data, data;	

	int i = 0;
	while (stream >> data) {
		if (data == FILE_PLAYER_SEPERATOR) {
			if (i < QUEST_LOG_CAPACITY && player_component->quest_log[i] != nullptr) {
				player_component->quest_log[i]->load(std::stringstream(quest_data));
				++i;
			}
		}
		else {
			quest_data += data + " ";
		}
	}
}

void load_spells(FileReader &file, Entity *player) {
	PlayerComponent *player_component = GetPlayer(player);
	MagicComponent *magic = GetMagic(player);

	if (file.exists(FILE_PLAYER_MAIN_SPELL)) magic->set_main(file.get_int(FILE_PLAYER_MAIN_SPELL));
	if (file.exists(FILE_PLAYER_SECONDARY_SPELL)) magic->set_secondary(file.get_int(FILE_PLAYER_SECONDARY_SPELL));
	if (!file.exists(FILE_PLAYER_SPELLS))
		return;

	std::stringstream stream(file.get_string(FILE_PLAYER_SPELLS));
	std::string data;
	int i = 0;
	while (i < TOTAL_PLAYER_SPELLS && stream >> player_component->known_spells[i]) { ++i; }
}

void load_player() {
	FileReader file(PLAYER_FILE);
	Entity *player = Environment::get().get_resource_manager()->get_player();

	load_position(file, player);
	load_items(file, player);
	load_buffs(file, player);
	load_info(file, player);
	load_quests(); // loads quest informaiton
	load_quest_log(file, player); // loads the save data for quests
	load_spells(file, player);

	if (file.exists(FILE_PLAYER_MAP_ID)) Environment::get().get_resource_manager()->load_map(file.get_int(FILE_PLAYER_MAP_ID), false);
}

void save_player() {
	std::ofstream file;
	file.open(PLAYER_FILE, std::ofstream::out | std::ofstream::trunc);
	Entity *player = Environment::get().get_resource_manager()->get_player();

	file << "# Player" << std::endl;

	CombatComponent *stats = GetCombat(player);
	PlayerComponent *player_component = GetPlayer(player);
	file << FILE_PLAYER_NAME << " " << player_component->name << std::endl;
	file << FILE_PLAYER_LEVEL << " " << player_component->level << std::endl;
	file << FILE_PLAYER_EXP << " " << player_component->exp << std::endl;
	file << FILE_PLAYER_HEALTH << " " << stats->health << std::endl;
	file << FILE_PLAYER_MANA << " " << stats->mana << std::endl;
	file << FILE_PLAYER_GOLD << " " << player_component->gold << std::endl;

	MagicComponent *magic = GetMagic(player);
	file << FILE_PLAYER_MAIN_SPELL << " " <<  magic->main_spell_id << std::endl;
	file << FILE_PLAYER_SECONDARY_SPELL << " " << magic->secondary_spell_id << std::endl;

	PositionComponent *position = GetPosition(player);
	file << FILE_PLAYER_POSITION << " " << position->pos_x << " " << position->pos_y << std::endl;

	file << FILE_PLAYER_INVENTORY << " ";
	for (auto &item : player_component->items) {
		file << item->get_type_id() << " ";
	}
	file << std::endl;

	file << FILE_PLAYER_EQUIPPED << " ";
	for (auto &item : player_component->equipped_items) {
		if (item == nullptr)
			file << -1 << " ";
		else
			file << item->get_type_id() << " ";
	}
	file << std::endl;


	file << FILE_PLAYER_BUFFS << " ";
	for (auto &buff : stats->buffs) {
		file << buff.timer.get_clock() << " "
			 << buff.timer.get_time() << " "
			 << buff.health << " "
			 << buff.mana << " "
		  	 << buff.damage << " "
			 << buff.armor << " "
			 << buff.speed << " "
			 << buff.luck << " "
			 << buff.icon_item_id << " ";
	}
	file << std::endl;

	file << FILE_PLAYER_QUEST_LOG << " ";
	for (auto &quest : player_component->quest_log) {
		if (quest != nullptr)
			quest->save(file);

		file << ". ";
	}
	file << std::endl;

	file << FILE_PLAYER_SPELLS << " ";
	for (auto &spell : player_component->known_spells) {
		file << spell << " ";
	}
	file << std::endl;

	file << FILE_PLAYER_MAP_ID << " " << Environment::get().get_resource_manager()->get_map()->get_id() << std::endl;

	file.close();
}