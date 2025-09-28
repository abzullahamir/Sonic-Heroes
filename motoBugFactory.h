#pragma once
#include "enemyFactory.h"
#include "motobug.h"

class motobugFactory : public enemyFactory {
public:
    char symbol() const override { return 'M'; }
    Enemy* create(float x, float y) const override {
        return new motobug(x, y);
    }
};
