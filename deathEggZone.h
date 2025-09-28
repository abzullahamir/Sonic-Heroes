#pragma once
#include "levels.h"
#include "playerFactory.h"
#include "sonicFactory.h"
#include "tailsFactory.h"
#include "knucklesFactory.h"
#include "grid.h"

class deathEggZone : public Levels {
private:
    static const int height = 14;
    static const int width = 300;
    const int pixels = 64;

    Grid* grid = nullptr;

    Texture wallTex, breakTex, springTex, crystalTex, backgroundTexture, platformTex, spikeTex;
    Sprite wallSprite, breakableSprite, springSprite, crystalSprite, backgroundSprite, platformSprite, spikeSprite;

    static const int maxCollectibles = 64;
    collectibles* collectiblesArray[maxCollectibles];
    int            totalCollectibles = 0;

    static const int maxEnemies = 32;
    Enemy* enemies[maxEnemies];
    int    totalEnemies = 0;

public:
    deathEggZone() {
        playerFactory[0] = new sonicFactory();
        playerFactory[1] = new knucklesFactory();
        playerFactory[2] = new tailsFactory();

        spawnX[0] = 100.f; spawnY[0] = 100.f;

        wallTex.loadFromFile("Data/wall.png");
        breakTex.loadFromFile("Data/break.png");
        springTex.loadFromFile("Data/spring.png");
        crystalTex.loadFromFile("Data/crystal.png");
        platformTex.loadFromFile("Data/platform.png");
        backgroundTexture.loadFromFile("Data/bg3.jpg");
        backgroundTexture.setRepeated(true);
        spikeTex.loadFromFile("Data/spike.png");

        wallSprite.setTexture(wallTex);
        breakableSprite.setTexture(breakTex);
        springSprite.setTexture(springTex);
        crystalSprite.setTexture(crystalTex);
        platformSprite.setTexture(platformTex);
        spikeSprite.setTexture(spikeTex);
        backgroundSprite.setTexture(backgroundTexture);
        int worldW = width * pixels;
        int worldH = height * pixels;
        backgroundSprite.setTextureRect({ 0, 0, worldW, worldH });

        loadLevel();
    }

    void loadLevel() {
        if (grid) delete grid;

        grid = new Grid(height, width);
        grid->loadFromFile("Data/level1.txt");
        loadCollectibles();
        loadEnemies();
    }

    void spawnPlayers(player* arrPlayers[]) override {
        for (int i = 0; i < numPlayers; ++i) {
            if (!playerFactory[i]) continue;

            player* existing = playerFactory[i]->getPlayer();
            if (!existing)
                existing = playerFactory[i]->createPlayer();

            arrPlayers[i] = existing;

            if (arrPlayers[i]) {
                arrPlayers[i]->setX(spawnX[i]);
                arrPlayers[i]->setY(spawnY[i]);
            }
            arrPlayers[i]->setDec(0.85f);
            arrPlayers[i]->setGrav(0.5f);
            arrPlayers[i]->setSpeed(3.f);
        }
    }

    void loadCollectibles() {
        totalCollectibles = 0;
        for (int row = 0; row < grid->getHeight(); ++row) {
            for (int col = 0; col < grid->getWidth(); ++col) {
                char sym = grid->getTile(col, row);
                collectibles* c = nullptr;

                for (int f = 0; f < numCollectibleTypes; ++f) {
                    if (collFactories[f]->symbol() == sym) {
                        c = collFactories[f]->create();
                        break;
                    }
                }

                if (c && totalCollectibles < maxCollectibles) {
                    float wx = col * grid->getPixels();
                    float wy = row * grid->getPixels();
                    c->init(wx, wy);
                    collectiblesArray[totalCollectibles++] = c;
                    grid->setTile(col, row, ' ');
                }
            }
        }
    }

    void loadEnemies() {
        totalEnemies = 0;
        for (int row = 0; row < grid->getHeight(); ++row) {
            for (int col = 0; col < grid->getWidth(); ++col) {
                char sym = grid->getTile(col, row);

                for (int f = 0; f < numEnemyTypes; ++f) {
                    if (enemyFactories[f]->symbol() == sym &&
                        totalEnemies < maxEnemies)
                    {
                        float wx = col * grid->getPixels();
                        float wy = row * grid->getPixels();
                        enemies[totalEnemies++] =
                            enemyFactories[f]->create(wx, wy);
                        grid->setTile(col, row, ' ');
                        break;
                    }
                }
            }
        }
    }

    collectibles** getCollectiblesArray() override { return collectiblesArray; }
    int            getNumCollectibles()   override { return totalCollectibles; }

    void render(RenderWindow& window, float offsetX)
    {
        // background
        backgroundSprite.setPosition(-offsetX * 0.5f, 0.f);
        window.draw(backgroundSprite);

        // tiles
        grid->render(window,
            offsetX,
            wallSprite,
            breakableSprite,
            springSprite,
            crystalSprite,
            platformSprite, spikeSprite);
    }

    Grid* getGrid() const override {
        return grid;
    }

    Enemy** getEnemies() { return enemies; }
    int     getNumEnemies() { return totalEnemies; }

    ~deathEggZone() override {
        delete grid;

        for (int i = 0; i < numPlayers; ++i)
            delete playerFactory[i];

        for (int i = 0; i < totalEnemies; ++i)
            delete enemies[i];

        for (int i = 0; i < totalCollectibles; ++i)
            delete collectiblesArray[i];
    }
};
