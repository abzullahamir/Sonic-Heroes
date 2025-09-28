#pragma once
#include "playerFactory.h"
#include "sonic.h"

class sonicFactory :public playerFactory {
public:
	sonicFactory() {
	}
	player* createPlayer() override {
		myPlayer = new Sonic();
		return myPlayer;
	}
	~sonicFactory() override = default;
};