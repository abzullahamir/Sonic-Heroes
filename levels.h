#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include "playerFactory.h"
#include "sonicFactory.h"
#include "tailsFactory.h"
#include "knucklesFactory.h"
#include "collectibles.h"
#include "enemy.h"
#include "collectibleFactory.h"
#include "ringFactory.h"
#include "extraLifeFactory.h"
#include "boostFactory.h"
#include "enemyFactory.h"
#include "batBrainFactory.h"
#include "beeBotFactory.h"
#include "motoBugFactory.h"
#include "crabMeatFactory.h"

class Levels {
private:

protected:
	RenderWindow* window;
	static int playerHealth;
	bool  invincible = false;
	Clock invClock;
	const float invDuration = 1.f;

	static const int numCollectibleTypes = 3;
	CollectibleFactory* collFactories[numCollectibleTypes];

	static const int numEnemyTypes = 4;
	enemyFactory* enemyFactories[numEnemyTypes];

	static const int numPlayers = 3;
	playerFactory* playerFactory[numPlayers];

public:
	float spawnX[numPlayers];
	float spawnY[numPlayers];

public:
	Levels() {
		for (int i = 0; i < numPlayers; i++) {
			playerFactory[i] = nullptr;
			spawnX[i] = 100;
			spawnY[i] = 64 * 9;
		}

		playerHealth = 15;

		// initializing collectible factories
		collFactories[0] = new RingFactory();
		collFactories[1] = new ExtraLifeFactory();
		collFactories[2] = new boostFactory();

		// initializing enemy factories
		enemyFactories[0] = new batBrainFactory();
		enemyFactories[1] = new beeBotFactory();
		enemyFactories[2] = new motobugFactory();
		enemyFactories[3] = new crabMeatFactory();
	}

	virtual void render(RenderWindow& window, float offsetX) = 0;
	virtual void loadLevel() = 0;
	virtual Grid* getGrid() const = 0;

	// spawns players at starting positions
	virtual void spawnPlayers(player* arrPlayers[]) {
		for (int i = 0; i < numPlayers; ++i) {
			if (!playerFactory[i]) continue;

			player* existing = playerFactory[i]->getPlayer();
			if (!existing)
				existing = playerFactory[i]->createPlayer();

			arrPlayers[i] = existing;

			if (arrPlayers[i]) {
				arrPlayers[i]->setX(spawnX[i]);
				arrPlayers[i]->setY(spawnY[i]);
			}
		}
	}

	virtual collectibles** getCollectiblesArray() {
		collectibles** c = nullptr;
		return c;
	}
	virtual int getNumCollectibles() { return 0; }

	virtual Enemy** getEnemies() {
		Enemy** enemy = nullptr;
		return enemy;
	}
	virtual int getNumEnemies() { return 0; }

	// taking damage
	void takePlayerDamage() {
		if (invincible) return;
		--playerHealth;
		invincible = true;
		invClock.restart();
	}

	// updating invincibility timer
	void updatePlayerInvincibility() {
		if (invincible && invClock.getElapsedTime().asSeconds() >= invDuration)
			invincible = false;
	}

	bool isPlayerInvincible() const {
		return invincible && invClock.getElapsedTime().asSeconds() < invDuration;
	}

	int getPlayerHealth() const { return playerHealth; }

	// time since damage taken
	float getInvTime() const {
		return invClock.getElapsedTime().asSeconds();
	}

	// increase life
	void addLife() {
		playerHealth += 15;
	}

	virtual ~Levels() {
		for (int i = 0; i < numCollectibleTypes; ++i)
			delete collFactories[i];
		for (int i = 0; i < numEnemyTypes; ++i)
			delete enemyFactories[i];
	}

	virtual void updateZone(float dt, player* leader) {}
	virtual bool isLevelComplete() { return false; }
};

int Levels::playerHealth = 15;
