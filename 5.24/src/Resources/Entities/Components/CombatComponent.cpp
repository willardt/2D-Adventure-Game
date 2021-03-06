#include <cstdlib>

#include "CombatComponent.h"

#include "Entity.h"
#include "PositionComponent.h"
#include "PlayerComponent.h"
#include "EnemyComponent.h"

#include "Environment.h"
#include "Window.h"
#include "ResourceManager.h"

#include "Text_Timed.h"

#define HEALTH_BAR_HEIGHT 8
#define MANA_BAR_HEIGHT 6

#define HEALTH_BAR_COLOR {225, 55, 55, 150}
#define MANA_BAR_COLOR {100, 185, 225, 150}

#define DAMAGE_TEXT_COLOR {225, 55, 55, 150}
#define DAMAGE_TEXT_WRAP_LENGTH 1000
#define DAMAGE_TEXT_FTSIZE 24
#define DAMAGE_TEXT_DISPLAY_TIME 400

#define ITEM_DROP_RANGE 50

#define BUFF_BAR_X 50
#define BUFF_BAR_Y 50
#define BUFF_BAR_HEIGHT 8
#define BUFF_BAR_COLOR {50, 75, 100, 255}

CombatComponent::CombatComponent(Entity *entity_, int max_health_, int max_mana_, int damage_, int armor_, int hps_, int mps_, int drain_, int luck_, int exp_, int combat_time) :
	Component		( entity_ ),
	max_health		( max_health_ ),
	health			( max_health_ ),
	max_mana		( max_mana_ ),
	mana			( max_mana_ ),
	damage			( damage_ ),
	armor			( armor_ ),
	hps				( hps_ ),
	mps				( mps_ ),
	drain			( drain_ ),
	luck			( luck_ ),
	exp				( exp_ ),
	in_combat       ( false ),
	combat_timer	( combat_time ),
	hps_mps_timer   ( 1000 )
{}

CombatComponent::CombatComponent(Entity *new_entity, const CombatComponent &rhs) :
	Component		( new_entity ),
	max_health		( rhs.max_health ),
	health			( rhs.health ),
	max_mana		( rhs.max_mana ),
	mana			( rhs.mana ),
	damage			( rhs.damage ),
	armor			( rhs.armor ),
	hps				( rhs.hps ),
	mps				( rhs.mps ),
	drain			( rhs.drain ),
	luck			( rhs.luck ),
	exp				( rhs.exp ),
	in_combat       ( rhs.in_combat ),
	combat_timer    ( rhs.combat_timer ),
	hps_mps_timer   ( rhs.hps_mps_timer )
{}

CombatComponent *CombatComponent::copy(Entity *new_entity) const {
	return new CombatComponent(new_entity, *this);
}

void CombatComponent::update() {
	if (in_combat) {
		in_combat = !combat_timer.update();
	}

	if (hps_mps_timer.update()) {
		health += hps;
		if (health > max_health) {
			health = max_health;
		}
		mana += mps;
		if (mana > max_mana) {
			mana = max_mana;
		}
	}

	auto it = buffs.begin();
	while (it != buffs.end()) {
		if (it->timer.update()) {
			remove_buff(*it);
			it = buffs.erase(it);
		}
		else {
			++it;
		}
	}
}

const int CombatComponent::get_type() const {
	return COMPONENT_COMBAT;
}

void CombatComponent::apply_damage(Combat_Info &attacker_info, const SDL_Color &color) {
	start_combat();

	//armor
	int final_damage = attacker_info.damage -= armor;
	if (final_damage < 0) {
		final_damage = 0;
	}

	//luck
	if (rand() % 100 < attacker_info.luck) {
		final_damage *= 2;
	}
	
	//drain
	mana -= attacker_info.drain;
	if (mana < 0) {
		mana = 0;
	}

	// apply damage
	health -= final_damage;
	if (health <= 0) {
		death();
	}

	PositionComponent *position = GetPosition(entity);
	if (position) {
		Text_Timed *text = new Text_Timed(
			std::to_string(final_damage),
			color,
			DAMAGE_TEXT_FTSIZE,
			DAMAGE_TEXT_WRAP_LENGTH,
			position->rect.x + rand() % position->rect.w,
			position->rect.y - rand() % position->rect.h,
			DAMAGE_TEXT_DISPLAY_TIME
		);
		Environment::get().get_resource_manager()->add_text(text);
	}
}

void CombatComponent::draw_health() {
	if (!in_combat) {
		return;
	}

	PositionComponent *position = GetPosition(entity);
	if (!position) {
		return;
	}

	SDL_Rect health_bar = { position->rect.x - int(position->rect.w / 4), position->rect.y - position->rect.h, 0, HEALTH_BAR_HEIGHT };
	SDL_Rect mana_bar = { health_bar.x, health_bar.y + HEALTH_BAR_HEIGHT, 0, MANA_BAR_HEIGHT };

	float percent = 0;
	if (health > 0) {
		int width = position->rect.w + position->rect.w / 2;
		percent = (float)health / (float)max_health;
		health_bar.w = int(percent * width);

		if (mana > 0) {
			percent = (float)mana / (float)max_mana;
			mana_bar.w = int(percent * width);
		}
	}

	Environment::get().get_window()->get_renderer()->draw_rect(health_bar, HEALTH_BAR_COLOR, DRAW_RECT_CAMERA);
	Environment::get().get_window()->get_renderer()->draw_rect(mana_bar, MANA_BAR_COLOR, DRAW_RECT_CAMERA);
}

void CombatComponent::start_combat() {
	in_combat = true;
	combat_timer.reset();
}

void CombatComponent::death() {
	if (PlayerComponent *player = GetPlayer(entity)) {

		return;
	}

	EnemyComponent *enemy = GetEnemy(entity);
	if (enemy) {
		PlayerComponent *player = GetPlayer(Environment::get().get_resource_manager()->get_player());
		player->add_exp(exp);
		drop_items(enemy);
		enemy->death();
		entity->destroy();
	}
}

void CombatComponent::drop_items(EnemyComponent *enemy) {
	for (auto &drop : enemy->drop_table) {
		if (rand() % 100 < drop.drop_chance) {
			PositionComponent *position = GetPosition(entity);
			float x = position->pos_x + rand() % ITEM_DROP_RANGE - rand() % ITEM_DROP_RANGE;
			float y = position->pos_y + rand() % ITEM_DROP_RANGE - rand() % ITEM_DROP_RANGE;
			Environment::get().get_resource_manager()->create_entity(TYPE_ITEM, drop.id, x, y);
		}
	}
}

void CombatComponent::add_buff(Buff_Info buff, bool new_buff) {
	health += buff.health;
	mana += buff.mana;
	damage += buff.damage;
	armor += buff.armor;
	luck += buff.luck;
	if (PositionComponent *position = GetPosition(entity)) {
		position->speed += buff.speed;
	}

	if (new_buff) {
		buff.timer.reset();
	}

	buffs.push_back(buff);
}

void CombatComponent::remove_buff(Buff_Info &buff) {
	health -= buff.health;
	mana -= buff.mana;
	damage -= buff.damage;
	armor -= buff.armor;
	luck -= buff.luck;
	if (PositionComponent *position = GetPosition(entity)) {
		position->speed -= buff.speed;
	}
}

void CombatComponent::draw_buffs() {
	int y = BUFF_BAR_Y;
	for (auto &buff : buffs) {
		int percent = int(((float)buff.timer.get_clock() / (float)buff.timer.get_time()) * 100);
		SDL_Rect buff_bar = { BUFF_BAR_X, y, 100 - percent, BUFF_BAR_HEIGHT };
		Environment::get().get_window()->get_renderer()->draw_rect(buff_bar, BUFF_BAR_COLOR, DRAW_RECT_FULL);
		SDL_Rect buff_icon = { BUFF_BAR_X - 30, y - 5, 20, 20 };
		Environment::get().get_window()->get_renderer()->render(Environment::get().get_resource_manager()->get_texture_info(TYPE_ITEM, buff.icon_item_id), buff_icon, true);
		y += BUFF_BAR_HEIGHT + 5;
	}
}