#pragma once

#include <SFML/Graphics.hpp>   
#include <SFML/Audio.hpp>      
#include <SFML/Window.hpp>     
#include <iostream>            

#include "levels.h"            
#include "player.h"    
#include "grid.h"
#include "collectibles.h"

using namespace std;
using namespace sf;

class runGame {
private:
    float cameraOffsetX = 0.f;
    static const int numPlayers = 3;

    RenderWindow* window;
    Levels* level;
    playerFactory* playerFactory[numPlayers];
    player* players[numPlayers];
    Music levelMusic;

    int leaderIndex = 0; // active player
    bool zHeld = false;  // debounce Z for switching
    const float followGap[2] = { 50.f, 75.f }; // space between followers

    // carry offsets for Tails
    const float carryOffsetX[2] = { 0.f, 30.f };
    const float carryOffsetY[2] = { 90.f, 180.f };

    bool levelDone = false;      // reached end of level
    bool quitByUser = false;     // player closed window
    bool sessionOver;            // used to stop game loop

    // HUD setup
    static const int MAX_HP = 30;
    CircleShape hpDot[MAX_HP];
    Font hudFont;
    Text timerText;
    Clock levelClock;
    int score;
    Text scoreText;

public:
    runGame(RenderWindow* win, Levels* lvl) : window(win), level(lvl) {
        for (int i = 0; i < numPlayers; ++i)
            players[i] = nullptr;

        // setup health HUD
        for (int i = 0; i < MAX_HP; ++i) {
            hpDot[i].setRadius(10.f);
            hpDot[i].setFillColor(Color::Green);
            hpDot[i].setOrigin(0.f, 0.f);
        }

        // load font for timer and score
        if (!hudFont.loadFromFile("Data/arial.ttf"))
            cerr << "Could not load Data/arial.ttf\n";

        timerText.setFont(hudFont);
        timerText.setCharacterSize(24);
        timerText.setFillColor(Color::White);

        score = 0;
        scoreText.setFont(hudFont);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(1200.f - 100.f, 40.f);
        scoreText.setString("Score: 0");
    }

    // set up level, music, and players
    void initialize() {
        quitByUser = false;
        levelDone = false;
        sessionOver = false;

        level->loadLevel();
        levelClock.restart();

        levelMusic.openFromFile("Data/labrynth.ogg");
        levelMusic.setLoop(true);
        levelMusic.play();

        level->spawnPlayers(players);
        for (int i = 0; i < numPlayers; ++i)
            if (players[i]) players[i]->setControlled(i == leaderIndex);
    }

    // main game loop
    void run() {
        Clock frameClock;
        while (window->isOpen() && !sessionOver) {
            float dt = frameClock.restart().asSeconds();

            handleInput();                    // player input
            update();                         // game logic
            level->updateZone(dt, players[leaderIndex]);
            resolveCollisions();             // handle tile/enemy/item collisions

            window->clear(Color::Black);
            render();                         // draw everything
            window->display();
        }

        levelMusic.stop(); // music ends when loop ends
    }

    // process user input
    void handleInput() {
        Event event;
        while (window->pollEvent(event)) {
            if (event.type == Event::Closed) {
                window->close();
                quitByUser = true;
                sessionOver = true;
            }

            // input for all players
            if (event.type == Event::KeyPressed || event.type == Event::KeyReleased) {
                for (int i = 0; i < numPlayers; ++i)
                    if (players[i]) players[i]->handleInput();
            }

            // escape = quit
            if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                window->close();
                quitByUser = true;
                sessionOver = true;
            }

            // Z key = switch leader
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Z && !zHeld) {
                zHeld = true;
                switchLeader();
            }
            if (event.type == Event::KeyReleased && event.key.code == Keyboard::Z)
                zHeld = false;
        }
    }

    // clamps camera to map bounds
    float clampFloat(float current, float lo, float hi) {
        if (current < lo) return lo;
        if (current > hi) return hi;
        return current;
    }

    // per-frame game logic
    void update() {
        level->updatePlayerInvincibility();

        player* leader = players[leaderIndex];
        if (leader) leader->update(*level->getGrid());

        bool tailsCarrying = (leader && leader->isCarrier());

        // clear all hanging state
        for (int i = 0; i < numPlayers; ++i)
            if (i != leaderIndex && players[i])
                players[i]->setHanging(false);

        // if Tails is carrying, teleport followers
        if (tailsCarrying) {
            int slot = 0;
            for (int i = 0; i < numPlayers; ++i) {
                if (i == leaderIndex || !players[i]) continue;
                player* follower = players[i];
                follower->setControlled(false);
                follower->setHanging(true);
                follower->setX(leader->getX() + carryOffsetX[slot]);
                follower->setY(leader->getY() + carryOffsetY[slot]);
                follower->update(*level->getGrid());
                ++slot;
            }
        }
        // otherwise, they follow normally
        else {
            int fIdx = 0;
            for (int i = 0; i < numPlayers; ++i)
                if (i != leaderIndex && players[i])
                    followAI(players[i], leader, followGap[fIdx++]);
        }

        // check if leader falls off screen
        float mapBottom = level->getGrid()->getHeight() * level->getGrid()->getPixels();
        if (leader && leader->getY() > mapBottom)
            sessionOver = true;

        // if follower falls, reset them
        for (int i = 0; i < numPlayers; ++i) {
            if (i == leaderIndex) continue;
            player* f = players[i];
            if (f && f->getY() > mapBottom) {
                float gap = followGap[(i < leaderIndex ? i : i - 1)];
                f->setX(leader->getX() - gap);
                f->setY(0.f);
                f->update(*level->getGrid());
            }
        }

        // update enemies
        for (int e = 0; e < level->getNumEnemies(); ++e)
            level->getEnemies()[e]->update(*level->getGrid(), leader);

        // update camera to follow leader
        if (leader) {
            float cx = leader->getX() + leader->getHitbox().getWidth() * 0.5f;
            float winW = window->getSize().x;
            float worldW = level->getGrid()->getWidth() * level->getGrid()->getPixels();
            cameraOffsetX = clampFloat(cx - winW * 0.5f, 0.f, worldW - winW);
        }

        // check for level end
        if (leader) {
            float goalX = level->getGrid()->getWidth() * level->getGrid()->getPixels() - leader->getWidth();
            if (leader->getX() >= goalX) {
                levelDone = true;
                sessionOver = true;
            }
        }

        if (level->isLevelComplete()) {
            levelDone = true;
            sessionOver = true;
        }
    }

    // resolve damage, collectibles, and overlap events
    void resolveCollisions() {
        Grid* grid = level->getGrid();
        player* leader = players[leaderIndex];

        if (leader) {
            hitBox& hb = leader->getHitbox();
            float midX = (hb.getLeft() + hb.getRight()) * 0.5f;
            float footY = hb.getBottom();
            int col = int(midX / grid->getPixels());
            int row = int(footY / grid->getPixels());

            if (grid->getTile(col, row) == 'x')
                level->takePlayerDamage();
        }

        // handle collectibles
        collectibles** collArr = level->getCollectiblesArray();
        int count = level->getNumCollectibles();
        for (int i = 0; i < count; ++i) {
            collectibles* c = collArr[i];
            if (!c) continue;

            bool removed = c->update(*grid);

            for (int j = 0; j < numPlayers && !removed; ++j) {
                if (players[j] && c->overlaps(players[j]->getHitbox())) {
                    if (c->isLife()) level->addLife();
                    else c->onCollect(*players[j]);
                    c->despawn(*grid);
                    removed = true;
                }
            }

            if (removed) {
                int pts = c->getPointValue();
                if (pts > 0) {
                    score += pts;
                    scoreText.setString("Score: " + to_string(score));
                }
                delete c;
                collArr[i] = nullptr;
            }
        }

        // check enemy collisions
        if (leader) {
            hitBox& phb = leader->getHitbox();
            for (int i = 0; i < level->getNumEnemies(); ++i) {
                Enemy* en = level->getEnemies()[i];
                if (!en->isAlive() || level->isPlayerInvincible()) continue;

                hitBox& ehb = en->getHitbox();
                if (ehb.intersects(phb))
                    level->takePlayerDamage();
            }
        }

        // end session on death
        if (level->getPlayerHealth() <= 0)
            sessionOver = true;
    }

    // render everything in one frame
    void render() {
        level->render(*window, cameraOffsetX);

        for (int i = 0; i < level->getNumCollectibles(); ++i)
            if (level->getCollectiblesArray()[i])
                level->getCollectiblesArray()[i]->draw(*window, cameraOffsetX);

        for (int i = 0; i < level->getNumEnemies(); ++i)
            level->getEnemies()[i]->draw(*window, cameraOffsetX);

        for (int i = 0; i < numPlayers; ++i) {
            player* p = players[i];
            if (!p) continue;

            if (i == leaderIndex && level->isPlayerInvincible()) {
                float t = level->getInvTime();
                Color c = (int(t * 10) % 2) ? Color::Transparent : Color::White;
                p->setColor(c);
            }
            else {
                p->setColor(Color::White);
            }

            p->draw(*window, cameraOffsetX);
        }

        drawHUD();
    }

    // switch current leader
    void switchLeader() {
        players[leaderIndex]->setControlled(false);
        leaderIndex = (leaderIndex + 1) % numPlayers;
        players[leaderIndex]->setControlled(true);
    }

    // follow logic for AI players
    void followAI(player* follower, player* lead, float horizontalGap) {
        if (!follower || !lead) return;

        float targetX = lead->getX() - horizontalGap;
        float dx = targetX - follower->getX();
        const float tol = 5.f;

        float maxSpd = min(follower->getMaxSpeed(), lead->getMaxSpeed());

        if (dx > tol) follower->setVelocityX(maxSpd);
        else if (dx < -tol) follower->setVelocityX(-maxSpd);
        else follower->setVelocityX(0.f);

        if (!lead->isCarrier() && !lead->isOnGround() && follower->isOnGround())
            follower->jump();

        follower->update(*level->getGrid());
    }

    bool levelCleared() const { return levelDone; }
    bool wasQuit() const { return quitByUser; }

    // draw health and timer HUD
    void drawHUD() {
        int hp = level->getPlayerHealth();
        if (hp > MAX_HP) hp = MAX_HP;

        const float startX = 10.f, startY = 10.f, gap = 25.f;
        for (int i = 0; i < hp; ++i) {
            hpDot[i].setPosition(startX + i * gap, startY);
            window->draw(hpDot[i]);
        }

        float elapsed = levelClock.getElapsedTime().asSeconds();
        int mm = static_cast<int>(elapsed) / 60;
        int ss = static_cast<int>(elapsed) % 60;

        string mmStr = (mm < 10 ? "0" : "") + to_string(mm);
        string ssStr = (ss < 10 ? "0" : "") + to_string(ss);
        timerText.setString(mmStr + ":" + ssStr);

        timerText.setPosition(1200.f - 100.f, 10.f);
        window->draw(timerText);
        window->draw(scoreText);
    }

    int getScore() { return score; }
};
