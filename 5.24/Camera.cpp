#include "Camera.h"

#include "Environment.h"
#include "Clock.h"
#include "WindowManager.h"
#include "ResourceManager.h"

#include "Event.h"

#include "Entity.h"
#include "PositionComponent.h"
#include "MoveableComponent.h"

#include "FileReader.h"

Camera::Camera() {
	_x = 0, _y = 0;

	_speed = _SPEED;
	FileReader file(_FILE_PATH);
	if (file.exists(FILE_CAMERA_SPEED)) _speed = file.get_double(FILE_CAMERA_SPEED);

	_is_locked = true;
}

void Camera::toggle() {
	_is_locked = !_is_locked;
}

void Camera::move(int dir) {
	if (_is_locked) {
		return;
	}

	if (dir == Event::UP) {
		_y -= _speed * Environment::get().getClock()->getTime();
	}
	else if (dir == Event::DOWN) {
		_y += _speed * Environment::get().getClock()->getTime();
	}
	else if (dir == Event::LEFT) {
		_x -= _speed * Environment::get().getClock()->getTime();
	}
	else if (dir == Event::RIGHT) {
		_x += _speed * Environment::get().getClock()->getTime();
	}
}

void Camera::center(Entity *entity) {
	if (!_is_locked) {
		return;
	}

	int width_half = Environment::get().getWindowManager()->getWindow()->getWidthHalf();
	int height_half = Environment::get().getWindowManager()->getWindow()->getHeightHalf();
	int max_width = Environment::get().getResourceManager()->getMap()->getWidth() - Environment::get().getWindowManager()->getWindow()->getWidth();
	int max_height = Environment::get().getResourceManager()->getMap()->getHeight() - Environment::get().getWindowManager()->getWindow()->getHeight();

	PositionComponent *position = GetPosition(entity);
	MoveableComponent *moveable = GetMoveable(entity);

	if (position) {
		_x = position->pos_x - width_half;
		_y = position->pos_y - height_half;
	}
	if (moveable) {
		_x = moveable->pos_x - width_half;
		_y = moveable->pos_y - height_half;
	}

	if (_x < 0) {
		_x = 0;
	}
	else if (_x > (max_width)) {
		_x = max_width;
	}
	if (_y < 0) {
		_y = 0;
	}
	else if (_y > (max_height)) {
		_y = max_height;
	}
}