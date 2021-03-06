#include "Level.h"

void Level::init(Screen &screen, int width, int height) {
	this->screen = &screen;
	this->width = width;
	this->height = height;
	generateLevel();
	endGame = false;
	score = 0;
	killCount = 0;
}

void Level::generateLevel() {
	//Adds asteroids to the level
	for (int i = 0; i < 400; i++) {
		spawnAsteroid();
	}

	//Adds a player to the level
	player = new Player();
	player->setSize(50);
	player->setPosition(100, 100);
	player->setLevel(width, height);
	screen->addPlayer(player);
}

void Level::spawnAsteroid() {

	/*
	This method puts the asteroids in a space where they are not touching other asteroids.
	If it cant find a free space in 20 attemps they it doesnt spawn that asteroids.
	*/

	bool freeSpace = false;
	
	float x, y;
	float rot = rand() % 360;
	float size = (rand() % 100) + 60;
	float xSpeed = (rand() % 3) - 1;
	float ySpeed = (rand() % 3) - 1;

	int spawnCounter = 0;

	while (!freeSpace) {
		if (screen->getEntityListSize() == 0) freeSpace = true;

		spawnCounter++;
		if (spawnCounter > 20) return;

		x = rand() % width;
		y = rand() % height;
		bool hit = false;

		for (int i = 0; i < screen->getEntityListSize(); i++) {

			float xDist = x - screen->getEntityAt(i)->getXa();
			float yDist = y - screen->getEntityAt(i)->getYa();
			float distance = sqrtf((xDist * xDist) + (yDist * yDist));
			float radiusTotal = (size + screen->getEntityAt(i)->getWidth()) / 2;

			if (radiusTotal > distance) hit = true;
		}

		if (hit == false) freeSpace = true;
	}

	Asteroid* temp = new Asteroid(x, y, size, size);
	temp->init();
	temp->asteroidSprite->setSize(size, size);
	temp->setRotation(rot);
	temp->setVelocity(xSpeed, ySpeed);
	temp->setLevel(width, height);
	screen->addEntity(temp);
}

void Level::update() {
	//Updates the camera position to follow the player
	screen->camera->setPosition(player->getXa() - (ofGetWindowWidth() / 2), player->getYa() - (ofGetWindowHeight() / 2));
	
	//Checks for collision between objects
	collision();

	coolDown++;

	//Creates a cooldown timer for projectiles
	if (coolDown >= player->getCoolDown()) {
		if (player->keys->SPACE) {
			Projectile* p = new Projectile(player->getXa(), player->getYa() - 3, player->getRotation());
			screen->addProjectile(p);
			coolDown = 0;
		}
	}
}

void Level::collision() {

	//Asteroid and Asteroid Collision
	std::vector<Entity*>* entityList = screen->getEntityList();
	for (int i = 0; i < entityList->size() - 1; i++) {
		if (entityList->size() == 0) return;
		for (int j = i + 1; j < entityList->size(); j++) {
			Entity* entityA = entityList->at(i);
			Entity* entityB = entityList->at(j);

			if (!entityA->isCollidable()) continue;
			if (!entityB->isCollidable()) continue;

			float xDist = entityA->getXa() - entityB->getXa();
			float yDist = entityA->getYa() - entityB->getYa();

			float distance = sqrtf((xDist * xDist) + (yDist * yDist));

			if (distance < (entityA->getWidth() / 2) + (entityB->getWidth() / 2)) {
				calcCollision(entityA, entityB);
			}
		}
	}

	//Projectile and Asteroid Collision
	std::vector<Projectile*>* projectileList = screen->getProjectileList();
	for (int i = 0; i < projectileList->size(); i++) {
		for (int j = 0; j < entityList->size(); j++) {
			if (!entityList->at(j)->isCollidable()) continue;
			
			Projectile* p1 = projectileList->at(i);
			Entity* e1 = entityList->at(j);

			float xDist = e1->getXa() - p1->getXa();
			float yDist = e1->getYa() - p1->getYa();
			float distance = sqrtf((xDist * xDist) + (yDist * yDist));

			if (distance <  (e1->getWidth() / 2)) {
				p1->remove();
				e1->remove();

				score += e1->getWidth();
				killCount++;

				//Creates an emitter to show the the asteroid is destroyed 

				screen->addEmitter(new ParticleEmitter(e1->getXa(), e1->getYa(), 200));
				
				//Creates two new asteroids of the original one is large enough

				if (e1->getWidth() / 2 < 50) continue;

				Asteroid* a1 = new Asteroid(e1->getXa() + e1->getWidth() / 4, e1->getYa() + e1->getWidth() / 4, e1->getWidth() / 2, e1->getHeight() / 2);
					
				a1->init();
				a1->asteroidSprite->setSize(e1->getWidth() / 2, e1->getHeight() / 2);
				a1->setRotation(e1->getRotation());
				a1->setVelocity(e1->getVelX(), e1->getVelY() * -1);
				a1->setLevel(width, height);

				screen->addEntity(a1);

				Asteroid* a2 = new Asteroid(e1->getXa() - e1->getWidth() / 4, e1->getYa() - e1->getWidth() / 4, e1->getWidth() / 2, e1->getHeight() / 2);

				a2->init();
				a2->asteroidSprite->setSize(e1->getWidth() / 2, e1->getHeight() / 2);
				a2->setRotation(e1->getRotation() * -1);
				a2->setVelocity(e1->getVelX() * -1, e1->getVelY());
				a2->setLevel(width, height);

				screen->addEntity(a2);
				
			}
		}
	}

	//Player and Asteroid Collision
	if (player->toRemove()) return;
	if (!player->isCollidable()) return;
	score++;
	for (int i = 0; i < entityList->size(); i++) {
		float xDist = entityList->at(i)->getXa() - player->getXa();
		float yDist = entityList->at(i)->getYa() - player->getYa();
		float distance = sqrtf((xDist * xDist) + (yDist * yDist));

		if (distance < (entityList->at(i)->getWidth() + player->getWidth() - 10) / 2) {
			//Ends the game if the player is hit
			screen->addEmitter(new ParticleEmitter(ofColor(244, 209, 66), player->getXa(), player->getYa(), 200));
			player->remove();
			endGame = true;
		}
	}
}

void Level::calcCollision(Entity* e1, Entity* e2) {

	/*
		Calculates the new velocities for the asteroids after they have hit each other.
		It then moves them once to try and prevent them getting suck inside each other.
	*/

	float xVel1 = (e1->getVelX() * (e1->getMass() - e2->getMass()) + (2 * e2->getMass() * e2->getVelX())) / (e1->getMass() + e2->getMass());
	float yVel1 = (e1->getVelY() * (e1->getMass() - e2->getMass()) + (2 * e2->getMass() * e2->getVelY())) / (e1->getMass() + e2->getMass());

	float xVel2 = (e2->getVelX() * (e2->getMass() - e1->getMass()) + (2 * e1->getMass() * e1->getVelX())) / (e1->getMass() + e2->getMass());
	float yVel2 = (e2->getVelY() * (e2->getMass() - e1->getMass()) + (2 * e1->getMass() * e1->getVelY())) / (e1->getMass() + e2->getMass());
	
	e1->setVelocity(xVel1, yVel1);
	e2->setVelocity(xVel2, yVel2);

	e1->move();
	e2->move();
}

void Level::destroyLevel() {
	//Sets all entities to be removed to reset the level.
	std::vector<Entity*>* entityList = screen->getEntityList();
	
	for (int i = 0; i < entityList->size(); i++) {
		entityList->at(i)->remove();
	}
}
