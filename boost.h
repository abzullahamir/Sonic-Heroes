#pragma once
#include "collectibles.h"
#include "player.h"

class boost : public collectibles {
public:
    boost() {
            const int W = 648;
        const int H = 648;

        if (!tex.loadFromFile("Data/boost.png")) {
            cerr << "Failed to load boost.png\n";
        }
        sprite.setTexture(tex);
        sprite.setScale(0.1, 0.1);

        box = hitBox(0, 0, W, H, 0, 0);

    }

    // When collected, call the players boost :
    void onCollect(player& p) override {
        p.applyBoost();  
    }
};
