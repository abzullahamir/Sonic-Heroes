#pragma once
#include "flyers.h"
#include "animation.h"
#include <string>

using namespace sf;
using namespace std;

class batBrain : public flyer {
private:
    static Texture flapStrip;
    static bool    flapLoaded;

    animation  animFlap;
    animation* curAnim;

    bool  chasing;
    float detectPx;

public:
    batBrain(float spawnX, float spawnY, float tileSize = 64.f)
        : flyer(
            flapStrip,
            spawnX, spawnY,
            3, 2.f, 64.f,
            32.f, 32.f
        )
        , curAnim(nullptr)
        , chasing(false)
        , detectPx(10.f * tileSize)
    {
        if (!flapLoaded) {
            flapStrip.create(4 * 34, 33);
            for (int i = 0; i < 4; ++i) {
                Image img;
                img.loadFromFile("Data/batbrain/bat-" + to_string(i) + ".png");
                flapStrip.update(img, i * 34, 0);
            }
            flapLoaded = true;
        }

        sprite.setTexture(flapStrip, true);
        sprite.setScale(2.f, 2.f);

        animFlap.setSprite(&sprite);
        for (int i = 0; i < 4; ++i)
            animFlap.addFrame(i * 34, 0, 34, 33);
        animFlap.setLoop(true);

        curAnim = &animFlap;
        curAnim->play();
    }

    void update(const Grid& grid, player* hero) override
    {
        if (health <= 0) return;

        /* animate flap */
        curAnim->update();

        /*  detection box */
        float dx = hero->getX() - curX;
        float dy = hero->getY() - curY;
        if (!chasing &&
            dx > -detectPx && dx < detectPx &&
            dy > -detectPx && dy < detectPx)
            chasing = true;
        if (chasing &&
            (dx <= -detectPx || dx >= detectPx ||
                dy <= -detectPx || dy >= detectPx))
            chasing = false;

        /* move */
        if (chasing) {
            float nx = curX + (dx > 0 ? speed : (dx < 0 ? -speed : 0));
            float ny = curY + (dy > 0 ? speed : (dy < 0 ? -speed : 0));
            setPos(nx, ny);
        }
        else {
            flyer::update(grid, hero);
            return;
        }

        //sync hit - box & collision 
        hb.updateHitbox(curX, curY);
        collideWithPlayer(hero);
    }
};

Texture batBrain::flapStrip;
bool    batBrain::flapLoaded = false;
