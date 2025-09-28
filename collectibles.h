
#pragma once
#include <SFML/Graphics.hpp>
#include "animation.h"
#include "hitbox.h"
#include "grid.h"
#include "levels.h"
#include "player.h"

using namespace sf;

class collectibles {
protected:
    float x, y;                 // world position 
    bool  collected = false;    // becomes true once picked up
    hitBox box;                

    Texture tex;                
    animation anim;           
    Sprite  sprite;

public:
    collectibles() : x(0), y(0), box(0, 0, 64, 64, 0, 0) {}   

    virtual ~collectibles() {}

    // initial set‑up after factory creates 
    virtual void init(float worldX, float worldY) {
        x = worldX; y = worldY;
        sprite.setPosition(x, y);
        box.updateHitbox(x, y);
    }

    // update – returns true if it vanished this frame 
    virtual bool update(Grid& grid) {
        if (collected) return true;          // already gone
        anim.update();                       // spin / blink
        sprite.setPosition(x, y);            // (if animation moved pivot)
        return false;
    }

    // draw only while visible 
    virtual void draw(RenderWindow& win, float camX) {
        if (!collected) {
            sprite.setPosition(x - camX, y);
            win.draw(sprite);
        }
    }

    // called once when a player touches 
    virtual void onCollect(player& p) = 0;   // effect (score, HP, power‑up)

    // collision test — AABB vs player’s active hit‑box 
    bool overlaps(const hitBox& hb) const { return !collected && box.intersects(hb); }

    // flag removal + let grid know the tile is now empty 
    void despawn(Grid& grid) {
        collected = true;
        int tileX = int(x) / grid.getPixels();
        int tileY = int(y) / grid.getPixels();
        grid.setTile(tileX, tileY, ' ');     // wipe from map
    }
   virtual bool isLife()const {
        return false;
    }
   virtual bool Collected() { return false; }
   virtual int getPointValue() const { return 0; }

};
