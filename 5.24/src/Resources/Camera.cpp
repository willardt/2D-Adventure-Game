#include "Camera.h"

#include "Environment.h"
#include "Clock.h"
#include "Window.h"
#include "ResourceManager.h"

#include "Entity.h"
#include "PositionComponent.h"

#include "FileReader.h"

#include "Globals.h"

#define CAMERA_ZOOM_SPEED 1.0f

Camera::Camera(int width, int height) :
	_x						( 0 ),
	_y						( 0 ),
	_speed					( _SPEED ),
	_uniform_scale			( 1.0f ),
	_uniform_rotation		( 0.0 ),
	_is_locked				( Environment::get().get_mode() == MODE_EDITOR ? false : true  ),
	_entity					( nullptr ),
	_rect					( {0, 0, width, height} )
{
	FileReader file(_FILE_PATH);
	if (file.exists(FILE_CAMERA_SPEED)) _speed = file.get_float(FILE_CAMERA_SPEED);
}

void Camera::toggle() {
	_is_locked = !_is_locked;
}

void Camera::move(int dir, float dis) {
	if (_is_locked) {
		return;
	}

	float distance = dis == 0 ? _speed : dis;

	if (dir == MOVE_UP) {
		_y -= float(distance * Environment::get().get_clock()->get_time());
	}
	else if (dir == MOVE_DOWN) {
		_y += float(distance * Environment::get().get_clock()->get_time());
	}
	else if (dir == MOVE_LEFT) {
		_x -= float(distance * Environment::get().get_clock()->get_time());
	}
	else if (dir == MOVE_RIGHT) {
		_x += float(distance * Environment::get().get_clock()->get_time());
	}
}

void Camera::update() {
	if (!_is_locked || !_entity) {
		return;
	}

	int width_half = Environment::get().get_window()->get_width_half();
	int height_half = Environment::get().get_window()->get_height_half();
	int max_width = Environment::get().get_resource_manager()->get_map()->get_rect().w - Environment::get().get_window()->get_width();
	int max_height = Environment::get().get_resource_manager()->get_map()->get_rect().h - Environment::get().get_window()->get_height();

	PositionComponent *position = GetPosition(_entity);

	if (!position) {
		return;
	}

	_x = position->pos_x - width_half;
	_y = position->pos_y - height_half;


	if (max_width < 0) {
		_x = max_width / 2;
	}
	else if (_x < 0) {
		_x = 0;
	}
	else if (_x > (max_width)) {
		_x = (float)max_width;
	}

	if (max_height < 0) {
		_y = max_height / 2;
	}
	else if (_y < 0) {
		_y = 0;
	}
	else if (_y > (max_height)) {
		_y = (float)max_height;
	}

	_rect.x = (int)_x;
	_rect.y = (int)_y;
}

void Camera::update(float x, float y) {
	if (_is_locked) {
		return;
	}

	_x += x;
	_y += y;
}

void Camera::center(Entity *entity) {
	_entity = entity;
}

void Camera::zoom(const float &amount) {
	scale(amount);
	if (amount < 0) {
		update(-CAMERA_ZOOM_SPEED, -CAMERA_ZOOM_SPEED);
	}
	else {
		update(CAMERA_ZOOM_SPEED, CAMERA_ZOOM_SPEED);
	}
}

void Camera::rotate(double angle) {
	_uniform_rotation += angle;
}

void Camera::set_rotation(double angle) {
	_uniform_rotation = angle;
}

void Camera::scale(float factor) {
	_uniform_scale += factor;
	SDL_RenderSetScale(Environment::get().get_window()->get_renderer()->get_renderer(), _uniform_scale, _uniform_scale);
}

void Camera::set_scale(float factor) {
	_uniform_scale = factor;
	SDL_RenderSetScale(Environment::get().get_window()->get_renderer()->get_renderer(), _uniform_scale, _uniform_scale);
}

bool Camera::get_locked() {
	return _is_locked;
}

void Camera::set_size(int width, int height) {
	_rect.w = width;
	_rect.h = height;
}