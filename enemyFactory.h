// enemyFactory.h
#pragma once
#include "enemy.h"

class enemyFactory {
public:
    virtual ~enemyFactory() {}
    virtual char symbol() const = 0;
    // Create a fresh Enemy at world x,y
    virtual Enemy* create(float x, float y) const = 0;
};
