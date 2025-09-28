#pragma once
#include "playerFactory.h"
#include "knuckles.h"

class knucklesFactory :public playerFactory {
public:
    
    knucklesFactory() {
    }
    player* createPlayer() override {
        myPlayer = new Knuckles();
        return myPlayer;
    }
    ~knucklesFactory()override = default;
};