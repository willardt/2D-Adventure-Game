#include "InputHandler.h"

#include "Environment.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "Window.h"

#include "PositionComponent.h"
#include "MagicComponent.h"
#include "PlayerComponent.h"
#include "InteractComponent.h"
#include "NPCComponent.h"

#include "Shop.h"

#define INTERACT_RANGE_W 96
#define INTERACT_RANGE_H 96

void move_entity(Entity *entity, int dir) {
	if (PositionComponent *position = GetPosition(entity)) {
		position->move(dir);
	}
}

void move_camera(int dir) {
	Environment::get().get_window()->get_camera()->move(dir);
}

void toggle_camera() {
	Environment::get().get_window()->get_camera()->toggle();
	if (Environment::get().get_window()->get_camera()->get_locked()) {
		Environment::get().get_window()->get_camera()->set_scale(1.0f);
	}
}

void cast_spell(float x, float y, bool main_spell) {
	Camera *camera = Environment::get().get_window()->get_camera();
	x = (x / camera->get_scale()) + camera->get_x();
	y = (y / camera->get_scale()) + camera->get_y();

	MagicComponent *magic = GetMagic(Environment::get().get_resource_manager()->get_player());
	if (magic) {
		PositionComponent *position = GetPosition(Environment::get().get_resource_manager()->get_player());
		if (main_spell) {
			magic->cast_main(x, y, position->pos_x + position->rect.w / 2, position->pos_y + position->rect.h / 2);
		}
		else {
			magic->cast_secondary(x, y, position->pos_x + position->rect.w / 2, position->pos_y + position->rect.h / 2);
		}
		magic->can_cast = false;
	}
}

void pickup_item() {
	Entity *player = Environment::get().get_resource_manager()->get_player();
	PositionComponent *player_position = GetPosition(player);
	PlayerComponent *player_component = GetPlayer(player);
	if (player_component->items.size() == MAX_ITEMS) {
		return;
	}
	
	auto items = Environment::get().get_resource_manager()->get_entities(TYPE_ITEM);
	for (auto it = items->begin(); it != items->end(); ++it) {
		if (PositionComponent *item_position = GetPosition(it->second)) {
			if (collision(player_position->rect, item_position->rect)) {
				player_component->inventory.add_item(it->second->get_type_id());
				delete it->second;
				items->erase(it);
				return;
			}
		}
	}
}

void interact() {
	Entity *player = Environment::get().get_resource_manager()->get_player();
	PositionComponent *player_position = GetPosition(player);

	const auto entity_vec = Environment::get().get_resource_manager()->get_map()->get_entity_grid()->get_cells(player_position->rect);

	for (auto &vec : entity_vec) {
		for (auto &e : *vec) {
			const int &type = e->get_type();
			if (type == TYPE_OBJECT || type == TYPE_NPC) {
				InteractComponent *interact = GetInteract(e);
				if (interact) {
					PositionComponent *object_position = GetPosition(e);
					if (interact_in_range(player_position->rect, object_position->rect)) {
						interact->interact();
						return;
					}
				}
			}
		}
	}
}

void open_shop() {
	Entity *player = Environment::get().get_resource_manager()->get_player();
	PositionComponent *player_position = GetPosition(player);

	const auto entity_vec = Environment::get().get_resource_manager()->get_map()->get_entity_grid()->get_cells(player_position->rect);

	for (auto &vec : entity_vec) {
		for (auto &e : *vec) {
			if (e->get_type() == TYPE_NPC) {
				if (NPCComponent *npc = GetNPC(e)) {
					if (npc->has_shop) {
						PositionComponent *position = GetPosition(e);
						if (interact_in_range(player_position->rect, position->rect)) {
							Shop shop(player, &GetPlayer(player)->items, e, &npc->item_ids);
							shop.open();
						}
					}
				}
			}
		}
	}
}

void execute_command() {
	if (!Environment::get().get_resource_manager()->get_textbox().get_show())
		Environment::get().get_resource_manager()->get_textbox().toggle();

	std::string input = "";
	Environment::get().get_input_manager()->get_text_input(&input, TEXT_INPUT_TEXTBOX);

	Environment::get().get_resource_manager()->get_textbox().print(input);
	luaL_dostring(Environment::get().get_lua()->get_state(), input.c_str());
}