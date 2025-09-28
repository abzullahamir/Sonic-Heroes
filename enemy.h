#pragma once
#include <SFML/Graphics.hpp>
#include "hitbox.h"
#include "grid.h"

using namespace sf;

class player;

class Enemy {
protected:
    Sprite   sprite;
    hitBox   hb;
    float    curX, curY;     // live position 
    float    x, y;        // immutable spawn
    int      health;

public:
    Enemy(const Texture& tex,
        float spawnX, float spawnY,
        int hp,
        float hbW, float hbH,
        float offX = 0.f, float offY = 0.f)
        : sprite(tex)
        , hb(spawnX, spawnY, hbW, hbH, offX, offY)
        , x(spawnX), y(spawnY)
        , curX(spawnX), curY(spawnY)
        , health(hp)
    {
        sprite.setPosition(curX, curY);
    }

    virtual ~Enemy() {}
    virtual void update(const Grid& grid, player* p) = 0;

    // only draw if still alive
    virtual void draw(RenderWindow& win, float camX)
    {
        if (health <= 0) return;
        sprite.setPosition(curX - camX, curY);
        win.draw(sprite);
        sprite.setPosition(curX, curY);
    }

    bool  isAlive()     const { return health > 0; }
    float getX()        const { return curX; }
    float getY()        const { return curY; }
    void  takeHit(int d = 1) { health -= d; }

    hitBox& getHitbox() { return hb; }

    void collideWithPlayer(player* p)
    {
        if (!p || health <= 0) return;
        if (!hb.intersects(p->getHitbox())) return;

        if (p->isRolling()) takeHit();
    }

protected:
    // update both cache + sprite
    void setPos(float nx, float ny)
    {
        curX = nx;  curY = ny;
        sprite.setPosition(nx, ny);
    }
};
