#pragma once
#include <SFML/Graphics.hpp>
#include "hitbox.h"

using namespace sf;

class Projectile {
private:
    Sprite sprite;
    hitBox hb;
    float  px, py;        
    float  vx;            
    bool   active;

public:
    Projectile() : px(0), py(0), vx(0), active(false) {}

    void fire(const Texture& tex,
        float startX, float startY,
        int dirSign,          
        float speed)
    {
        sprite.setTexture(tex, true);
        px = startX;
        py = startY;
        sprite.setPosition(px, py);

        hb = hitBox(px, py, 6.f, 6.f);   // red‑dot size hard‑coded
        vx = dirSign * speed;
        active = true;
    }

    void update()
    {
        if (!active) return;
        px += vx;
        sprite.setPosition(px, py);
        hb.updateHitbox(px, py);


        if (px < -50 || px > 5050) active = false;
    }

    void draw(RenderWindow& win, float camX)
    {
        if (!active) return;
        sprite.setPosition(px - camX, py);
        win.draw(sprite);
        sprite.setPosition(px, py);   // restore
    }

    bool isActive() const { return active; }
    hitBox& getHitbox() { return hb; }
};
