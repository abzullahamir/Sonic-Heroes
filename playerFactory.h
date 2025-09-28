#pragma once
#include "player.h"


class playerFactory {
	// base abstract functions to be overriden by the specific factories
protected:
	player* myPlayer;
public:
	virtual player* createPlayer() = 0;
	player* getPlayer() {
		return myPlayer;
	}
	virtual ~playerFactory() {
		delete myPlayer;
	}
};