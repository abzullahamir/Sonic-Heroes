
#pragma once
#include "enemyFactory.h"
#include "beeBot.h"

class beeBotFactory : public enemyFactory {
public:
    char symbol() const override { return 'E'; }
    Enemy* create(float x, float y) const override {
        return new BeeBot(x, y);
    }
};
