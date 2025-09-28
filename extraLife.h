#pragma once
#include "collectibles.h"
#include "player.h"
#include "levels.h"

class ExtraLife : public collectibles {
public:
    ExtraLife() {
        const int W = 676;
        const int H = 676;

        if (!tex.loadFromFile("Data/life.png")) {
            std::cerr << "Failed to load Data/life.png\n";
        }

        sprite.setTexture(tex);
        sprite.setScale(0.1, 0.1);
        // (optional) position it wherever you need:
        // sprite.setPosition(startX, startY);

        // Hit-box matches the full 676×676 sprite
        box = hitBox(0, 0, W, H, 0, 0);

    }

   void onCollect(player& p) override {  } // add a life to the player
    bool isLife() const override { return true; }
};

