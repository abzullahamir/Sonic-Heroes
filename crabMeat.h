#pragma once
#include "crawlers.h"
#include "projectile.h"
#include "animation.h"
#include <string>

using namespace sf;
using namespace std;

class CrabMeat : public crawler {
private:
    static Texture walkStrip;    static bool walkLoaded;
    static Texture shootStrip;   static bool shootLoaded;
    static Texture bulletTex;    static bool bulletLoaded;

    animation animWalk, animShoot;
    animation* curAnim;

    static const int maxProj = 8;
    Projectile    shots[maxProj];
    int           shootTimer = 0;
    static const int shootInterval = 300;

public:
    CrabMeat(float spawnX, float spawnY, float patrolRange = 64.f)
        : crawler(walkStrip, spawnX, spawnY,
            4, 1.5f, patrolRange,
            32.f, 32.f)
    {
        if (!walkLoaded) {
            walkStrip.create(4 * 44, 31);
            for (int i = 0, j = 1; i < 4; ++i, ++j) {
                Image img;
                img.loadFromFile("Data/crabmeat/crabmeat-" + to_string(j) + ".png");
                walkStrip.update(img, i * 44, 0);
            }
            walkLoaded = true;
        }

        if (!shootLoaded) {
            shootStrip.create(4 * 48, 30);
            for (int i = 0, j = 7; i < 4; ++i, ++j) {
                Image img;
                img.loadFromFile("Data/crabmeat/crabmeat-" + to_string(j) + ".png");
                shootStrip.update(img, i * 48, 0);
            }
            shootLoaded = true;
        }

        if (!bulletLoaded) {
            Image dot; dot.create(6, 6, Color::Red);
            bulletTex.loadFromImage(dot);
            bulletLoaded = true;
        }

        sprite.setTexture(walkStrip, true);
        animWalk.setSprite(&sprite);
        for (int i = 0; i < 4; ++i)
            animWalk.addFrame(i * 44, 0, 44, 31);
        animWalk.setLoop(true);

        animShoot.setSprite(&sprite);
        for (int i = 0; i < 4; ++i)
            animShoot.addFrame(i * 48, 0, 48, 30);
        animShoot.setLoop(true);

        curAnim = &animWalk;
        curAnim->play();
    }

    void update(const Grid& grid, player* hero) override
    {
        if (health <= 0) { updateBullets(hero); return; }

        // animate
        curAnim->update();

        // patrol
        crawler::update(grid, hero);

        // shoot
        if (++shootTimer >= shootInterval) {
            shootTimer = 0;
            fireShot();
            sprite.setTexture(shootStrip, true);
            curAnim = &animShoot;
            curAnim->play();
            shootTimer = -30;
        }

        // bullets
        updateBullets(hero);

        // revert anim
        if (shootTimer < 0 && ++shootTimer == 0) {
            sprite.setTexture(walkStrip, true);
            curAnim = &animWalk;
            curAnim->play();
        }
    }

private:
    void fireShot() {
        int dirSign = dir;
        for (int i = 0; i < maxProj; ++i) {
            if (!shots[i].isActive()) {
                shots[i].fire(bulletTex, curX, curY + 8.f,
                    dirSign, 4.f);
                break;
            }
        }
    }

    void updateBullets(player* hero) {
        for (int i = 0; i < maxProj; ++i) {
            shots[i].update();
            if (shots[i].isActive() &&
                shots[i].getHitbox().intersects(hero->getHitbox()))
            {
                shots[i] = Projectile();
            }
        }
    }

    void draw(RenderWindow& win, float camX) override {
        Enemy::draw(win, camX);
        for (int i = 0; i < maxProj; ++i)
            shots[i].draw(win, camX);
    }
};

Texture CrabMeat::walkStrip;    bool CrabMeat::walkLoaded = false;
Texture CrabMeat::shootStrip;   bool CrabMeat::shootLoaded = false;
Texture CrabMeat::bulletTex;    bool CrabMeat::bulletLoaded = false;
