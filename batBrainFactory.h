
#pragma once
#include "enemyFactory.h"
#include "batBrain.h"

class batBrainFactory : public enemyFactory {
public:
    char symbol() const override { return 'B'; }
    Enemy* create(float x, float y) const override {
        return new batBrain(x, y);
    }
};
