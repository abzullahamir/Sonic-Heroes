#pragma once
#include "enemyFactory.h"
#include "CrabMeat.h"

class crabMeatFactory : public enemyFactory {
public:
    char symbol() const override { return 'C'; }
    Enemy* create(float x, float y) const override {
        return new CrabMeat(x, y);
    }
};
