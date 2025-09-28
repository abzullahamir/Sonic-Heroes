#pragma once
#include "levels.h"
#include "playerFactory.h"
#include "sonicFactory.h"
#include "grid.h"
#include "hitbox.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;
using namespace sf;

class BossZone : public Levels {
private:
    Grid* grid = nullptr;
    Texture     wallTex, bgTex, bossTex;
    Sprite      wallSprite, bgSprite, bossSprite;

    hitBox      bossHitbox;
    float       bossX = 20 * 64.f;
    float       bossY = 2 * 64.f;
    float       bossVX = 200.f;
    int         bossHP = 20;

    float       attackTimer = 0.f;
    bool        diving = false;
    bool        ascending = false;
    int         targetCol = -1;
    bool bossDefeated = false;
    bool        crumbling = false;
    float       crumbleTimer = 0.f;

    Music       bossMusic;

public:
    BossZone() {
        // only Sonic
        playerFactory[0] = new sonicFactory();
        for (int i = 1; i < numPlayers; ++i)
            playerFactory[i] = nullptr;
        spawnX[0] = 100;
        spawnY[0] = 100;
        
        if (!bgTex.loadFromFile("Data/boss.jpg"))
            cerr << "BossZone: bg1.png missing\n";
        bgTex.setRepeated(true);
        bgSprite.setTexture(bgTex);

        if (!wallTex.loadFromFile("Data/wall.png"))
            cerr << "BossZone: wall.png missing\n";
        wallSprite.setTexture(wallTex);

        if (!bossTex.loadFromFile("Data/boss.png"))
            cerr << "BossZone: boss.png missing\n";
        bossSprite.setTexture(bossTex);
        bossSprite.setScale(2.f, 2.f);

        loadLevel();
    }

    void loadLevel() override {
        if (grid) delete grid;

     
        grid = new Grid(14, 40);
        grid->loadFromFile("Data/boss.txt");

       
        int w = grid->getWidth() * grid->getPixels();
        int h = grid->getHeight() * grid->getPixels();
        bgSprite.setTextureRect({ 0,0,w,h });

        //  boss hitbox
        bossSprite.setPosition(bossX, bossY);
        bossHitbox = hitBox(bossX, bossY, 48, 48, 8, 8);

        // boss lvl music
        if (bossMusic.openFromFile("Data/boss.mp3")) {
            bossMusic.setLoop(true);
            bossMusic.play();
        }
    }

    void render(RenderWindow& window, float offsetX) override {
        // background
        bgSprite.setPosition(-offsetX * 0.5f, 0);
        window.draw(bgSprite);

       
        grid->render(window,
            offsetX,
            wallSprite,  // walls
            wallSprite,  // breakables
            wallSprite,  // springs
            wallSprite,  // crystals
            wallSprite,  // platforms
            wallSprite); // spikes

        // boss or crumble
        if (crumbling) {
            float s = 2.f * (1 - crumbleTimer / 2.f);
            bossSprite.setScale(s, s);
            bossSprite.setColor((int(crumbleTimer * 10) % 2)
                ? Color::White
                : Color(255, 255, 255, 128));
        }
        else {
            bossSprite.setScale(2.f, 2.f);
            bossSprite.setColor(Color::White);
        }
        bossSprite.setPosition(bossX - offsetX, bossY);
        window.draw(bossSprite);
    }
    void updateZone(float dt, player* leader) override {
        //  Accumulate time until next dive
        attackTimer += dt;
        if (!diving && !ascending && attackTimer >= 5.f) {
            diving = true;
            attackTimer = 0.f;

            // Pick the tile under sonic
            hitBox& phb = leader->getHitbox();
            float centerX = (phb.getLeft() + phb.getRight()) * 0.5f;
            targetCol = int(centerX / grid->getPixels());
        }

        //  move down until you hit a wall tile
        if (diving) {
            bossY += bossVX * dt;
            bossHitbox.updateHitbox(bossX, bossY);

            int row = int(bossY / grid->getPixels());
            if (row >= grid->getHeight() ||
                grid->getTile(targetCol, row) == 'w')
            {
                // smash the brick and switch to ascend
                grid->setTile(targetCol, row, ' ');
                diving = false;
                ascending = true;
            }
        }
        //  return to start height
        else if (ascending) {
            bossY -= bossVX * dt;
            if (bossY <= 2 * grid->getPixels()) {
                bossY = 2 * grid->getPixels();
                ascending = false;
            }
            bossHitbox.updateHitbox(bossX, bossY);
        }

        // Collision vs. player
        if (leader) {
            hitBox& phb = leader->getHitbox();
            if (bossHitbox.intersects(phb)) {
                // if Sonic is falling onto the boss
                if (leader->getVelY() > 0.f) {
                    --bossHP;
                    crumbling = true;
                    crumbleTimer = 0.f;
                }
                else {
                    takePlayerDamage();
                }
            }
        }

        //  Crumble animation timer
        if (crumbling) {
            crumbleTimer += dt;
            if (crumbleTimer >= 2.f)
                crumbling = false;
        }
        if (bossHP <= 0 && !bossDefeated) {
            bossDefeated = true;
        }
    }
    bool isLevelComplete()  override {
        return bossDefeated;
    }

    Grid* getGrid() const override { return grid; }

    ~BossZone() override {
        delete grid;
        delete playerFactory[0];
        bossMusic.stop();
    }
   
};
