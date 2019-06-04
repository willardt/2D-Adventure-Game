
#include "EntityManager.h"
#include "TextureManager.h"
#include "Renderer.h"

#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

class ResourceManager {
public:
	ResourceManager(std::shared_ptr<Renderer> renderer);
	
	void updateEntities() { _entityManager->update(); }
	void renderEntities();

	void renderEntity(Entity *entity);

	void render();

	Entity *getEntity(int id) { return _entityManager->get(id); }
	Player *getPlayer()       { return _entityManager->getPlayer(); }
private:
	std::shared_ptr<EntityManager> _entityManager;
	std::shared_ptr<TextureManager> _textureManager;
	std::shared_ptr<Renderer> _renderer;
};

#endif