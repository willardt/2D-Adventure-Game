#include "Player.h"

#include "Component.h"
#include "PositionComponent.h"

#include "Environment.h"
#include "ResourceManager.h"

void Player::update() {
	Entity::update();

	
	// WARP
	if (PositionComponent *position = GetPosition(this)) {
		Map::Warp *warp = Environment::get().get_resource_manager()->get_map()->warp_collision(position->rect);
		if (warp != nullptr) {
			position->set(float(warp->to.x + (warp->to.w / 2) - (position->rect.w / 2)), float(warp->to.y + (warp->to.h / 2) - (position->rect.h / 2)));
			if(warp->to_id != Environment::get().get_resource_manager()->get_map()->get_id())
				Environment::get().get_resource_manager()->load_map(warp->to_id);
		}
	}
}