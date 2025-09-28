#pragma once
#include "enemy.h"
#include "player.h"

using namespace sf;

class crawler : public Enemy {
protected:
    float speed, range;
    int   dir;

public:
    crawler(const Texture& tex,
        float spawnX, float spawnY,
        int   hp,
        float spd,
        float patrolRange,
        float hbW,
        float hbH)
        : Enemy(tex, spawnX, spawnY, hp, hbW, hbH)
        , speed(spd), range(patrolRange), dir(1)
    {
        sprite.setScale(2.f, 2.f);
    }

    void update(const Grid& grid, player* p)
    {
        if (health <= 0) return;

        //  compute candidate X
        float nextX = curX + dir * speed;

        
         if (nextX > x + range) {
            nextX = x + range; dir = -1;
        }
        else if (nextX < x - range) {
            nextX = x - range; dir = 1;
        }

        //  apply movement
        setPos(nextX, curY);
        hb.updateHitbox(curX, curY);
        collideWithPlayer(p);
    }

};
