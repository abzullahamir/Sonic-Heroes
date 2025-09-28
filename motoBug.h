#pragma once
#include "crawlers.h"
#include "animation.h"
#include <string>

using namespace sf;
using namespace std;

class motobug : public crawler {
private:
    static Texture rightStrip;  static bool rightLoaded;
    static Texture leftStrip;   static bool leftLoaded;

    animation walkRight, walkLeft;
    animation* curAnim;

    float detectPx;

public:
    motobug(float spawnX, float spawnY, float tileSize = 64.f)
        : crawler(rightStrip, spawnX, spawnY,
            2, 1.5f, 0.f,
            32.f, 32.f)
        , curAnim(nullptr)
        , detectPx(10.f * tileSize)
    {
        if (!rightLoaded) {
            rightStrip.create(3 * 39, 28);
            for (int i = 0; i < 3; ++i) {
                Image img;
                img.loadFromFile("Data/motobug/motobug-" + to_string(i) + ".png");
                rightStrip.update(img, i * 39, 0);
            }
            rightLoaded = true;
        }

        if (!leftLoaded) {
            leftStrip.create(2 * 36, 28);
            for (int i = 0; i < 2; ++i) {
                Image img;
                img.loadFromFile("Data/motobugL-" + to_string(i) + ".png");
                leftStrip.update(img, i * 36, 0);
            }
            leftLoaded = true;
        }

        sprite.setTexture(rightStrip, true);

        walkRight.setSprite(&sprite);
        for (int i = 0; i < 3; ++i)
            walkRight.addFrame(i * 39, 0, 39, 28);
        walkRight.setLoop(true);

        walkLeft.setSprite(&sprite);
        for (int i = 0; i < 2; ++i)
            walkLeft.addFrame(i * 36, 0, 36, 28);
        walkLeft.setLoop(true);

        curAnim = &walkRight;
        curAnim->play();
    }

    void update(const Grid& grid, player* hero) override {
        if (health <= 0) return;

        // updating animation
        curAnim->update();

        // checking player range
        float dx = hero->getX() - curX;
        if (dx > -detectPx && dx < detectPx) {

            // face right and move
            if (dx > 0) {
                if (curAnim != &walkRight) {
                    sprite.setTexture(rightStrip, true);
                    curAnim = &walkRight;
                    curAnim->play();
                }
                setPos(curX + speed, curY);
            }

            // face left and move
            else {
                if (curAnim != &walkLeft) {
                    sprite.setTexture(leftStrip, true);
                    curAnim = &walkLeft;
                    curAnim->play();
                }
                setPos(curX - speed, curY);
            }

            // update hitbox and check collision
            hb.updateHitbox(curX, curY);
            collideWithPlayer(hero);
        }
    }
};

// static member definitions
Texture motobug::rightStrip;  bool motobug::rightLoaded = false;
Texture motobug::leftStrip;   bool motobug::leftLoaded = false;
