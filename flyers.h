#pragma once
#include "enemy.h"
#include "player.h"

using namespace sf;

class flyer : public Enemy {
protected:
    float speed, range;
    int   dir;

public:
    flyer(const Texture& tex,
        float spawnX, float spawnY,
        int hp, float spd, float patrolRange,
        float hbW, float hbH)
        : Enemy(tex, spawnX, spawnY, hp, hbW, hbH)
        , speed(spd), range(patrolRange), dir(1)
    {
    }

    void update(const Grid& grid, player* p) override
    {
        if (health <= 0) return;

        // back-and-forth on X around spawn x
        float nextX = curX + dir * speed;
        if (nextX > x + range) { nextX = x + range; dir = -1; }
        else if (nextX < x - range) { nextX = x - range; dir = 1; }

        setPos(nextX, curY);
        hb.updateHitbox(curX, curY);
        collideWithPlayer(p);
    }
};
