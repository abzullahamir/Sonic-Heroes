#pragma once
#include "flyers.h"
#include "projectile.h"
#include "animation.h"
#include <SFML/Graphics/Image.hpp>
#include <string>

using namespace sf;
using namespace std;

class BeeBot : public flyer {
private:
    static Texture flyStrip;      static bool flyStripLoaded;
    static Texture shootStrip;    static bool shootStripLoaded;
    static Texture redDotTex;     static bool redDotTexLoaded;

    animation animFly, animShoot;
    animation* curAnim;

    int   dirX = 1, dirY = 1;
    float dzX = 2.f, dzY = 2.f;
    float maxDX = 64.f, maxDY = 48.f;

    static const int maxProj = 8;
    Projectile shots[maxProj];
    int        shootTimer = 0;
    static const int shootInterval = 300;

public:
    BeeBot(float sx, float sy)
        : flyer(flyStrip, sx, sy, 5, 0, 0, 45.f, 28.f)
    {
        if (!flyStripLoaded) {
            flyStrip.create(3 * 45, 19);
            for (int i = 0; i < 3; ++i) {
                Image img;
                img.loadFromFile("Data/beebot/beebot-" + to_string(i) + ".png");
                flyStrip.update(img, i * 45, 0);
            }
            flyStripLoaded = true;
        }
        if (!shootStripLoaded) {
            shootStrip.create(5 * 36, 28);
            for (int i = 0; i < 5; ++i) {
                Image img;
                img.loadFromFile("Data/beebot/beebotS-" + to_string(i) + ".png");
                shootStrip.update(img, i * 36, 0);
            }
            shootStripLoaded = true;
        }
        if (!redDotTexLoaded) {
            Image dot; dot.create(6, 6, Color::Red);
            redDotTex.loadFromImage(dot);
            redDotTexLoaded = true;
        }

        sprite.setTexture(flyStrip, true);
        animFly.setSprite(&sprite);
        for (int i = 0; i < 3; ++i) animFly.addFrame(i * 45, 0, 45, 19);
        animFly.setLoop(true);

        animShoot.setSprite(&sprite);
        for (int i = 0; i < 5; ++i) animShoot.addFrame(i * 36, 0, 36, 28);
        animShoot.setLoop(true);

        curAnim = &animFly;
        curAnim->play();
    }

    void update(const Grid& grid, player* hero) override
    {
        if (health <= 0) { updateBullets(hero); return; }

        curAnim->update();

        float nx = curX + dirX * dzX;
        float ny = curY + dirY * dzY;
        if (nx > x + maxDX) { nx = x + maxDX; dirX = -1; }
        else if (nx < x - maxDX) { nx = x - maxDX; dirX = 1; }
        if (ny > y + maxDY) { ny = y + maxDY; dirY = -1; }
        else if (ny < y - maxDY) { ny = y - maxDY; dirY = 1; }
        setPos(nx, ny);
        hb.updateHitbox(curX, curY);

        if (++shootTimer >= shootInterval) {
            shootTimer = 0;
            fireShot(hero);
            sprite.setTexture(shootStrip, true);
            curAnim = &animShoot;
            curAnim->play();
            shootTimer = -30;
        }

        updateBullets(hero);
        collideWithPlayer(hero);

        if (shootTimer < 0 && ++shootTimer == 0) {
            sprite.setTexture(flyStrip, true);
            curAnim = &animFly;
            curAnim->play();
        }
    }
    void fireShot(player* hero) {
        int sign = (hero->getX() > curX) ? 1 : -1;
        for (int i = 0; i < maxProj; ++i) {
            if (!shots[i].isActive()) {
                shots[i].fire(redDotTex, curX, curY + 8.f, sign, 4.f);
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

Texture BeeBot::flyStrip;      bool BeeBot::flyStripLoaded = false;
Texture BeeBot::shootStrip;    bool BeeBot::shootStripLoaded = false;
Texture BeeBot::redDotTex;     bool BeeBot::redDotTexLoaded = false;
