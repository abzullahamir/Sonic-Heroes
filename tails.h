#pragma once
#include "player.h"
#include "animation.h"
#include "hitbox.h"
#include "grid.h"
#include <SFML/Audio.hpp>
#include <string>

using namespace sf;
using namespace std;

class Tails : public player {
private:
    // Physics variables
    float velocityX = 0.f;
    float speed = 10.f;
    float deceleration = 0.80f;
    bool isFacingRight = true;

    // Flight control
    static const int maxFlyFrames = 7 * 60;
    bool flying = false;
    const float baseFlightSec = 7.f;
    float flightDuration = baseFlightSec;
    Clock flightClock;
    bool prevFly = false;

    // Textures and animations
    Texture runRightTexture, runLeftTexture;
    Texture idleRightTexture, idleLeftTexture;
    Texture jumpRightTexture, jumpLeftTexture;
    Texture flyRightTexture, flyLeftTexture, hangRightTexture;

    animation animRunRight, animRunLeft;
    animation animIdleRight, animIdleLeft;
    animation animJumpRight, animJumpLeft;
    animation animFlyRight, animFlyLeft, animHangRight;

    animation* currentAnim = nullptr;

    // SFX (optional placeholders)
    SoundBuffer flapBuf;
    Sound flapSfx;

public:
    // Constructor: load animations and setup hitboxes
    Tails(float startX = 0, float startY = 100)
        : player(getDefaultTexture(), startX, startY, 2.5f, 2.5f)
    {
        float hbW = 20.5f * 2.5f;
        float hbH = 33.f * 2.5f;
        float offX = 33.f;
        float offY = 34.f;

        // Hitboxes
        groundBox = hitBox(startX, startY, hbW, hbH, offX, offY);
        airBox = hitBox(startX, startY + 15.f, 64.f, 83.50f, 39.0f, 25.0f);
        box = &groundBox;

        // Running animation (mirrored)
        runRightTexture.create(9 * 48, 48);
        for (int i = 0; i < 9; ++i) {
            Image img; img.loadFromFile("Data/tails/jog-" + to_string(i) + ".png");
            runRightTexture.update(img, i * 48, 0);
            animRunRight.addFrame(i * 48, 0, 48, 48);
        }
        animRunRight.setSprite(&sprite); animRunRight.setLoop(true);
        runLeftTexture = runRightTexture;
        animRunLeft = animRunRight; animRunLeft.setSprite(&sprite);

        // Idle animation
        idleRightTexture.loadFromFile("Data/tails/idel.png");
        animIdleRight.setSprite(&sprite); animIdleRight.addFrame(0, 0, 48, 48); animIdleRight.setLoop(true);
        idleLeftTexture = idleRightTexture;
        animIdleLeft = animIdleRight; animIdleLeft.setSprite(&sprite);

        // Jump animation (left is reversed)
        jumpRightTexture.create(5 * 48, 48);
        for (int i = 0; i < 5; ++i) {
            Image img; img.loadFromFile("Data/tails/jump-" + to_string(i) + ".png");
            jumpRightTexture.update(img, i * 48, 0);
            animJumpRight.addFrame(i * 48, 0, 48, 48);
        }
        animJumpRight.setSprite(&sprite);
        jumpLeftTexture = jumpRightTexture;
        for (int i = 4; i >= 0; --i)
            animJumpLeft.addFrame(i * 48, 0, 48, 48);
        animJumpLeft.setSprite(&sprite);

        // Flying animation (mirrored)
        flyRightTexture.create(3 * 56, 48);
        for (int i = 0; i < 3; ++i) {
            Image img; img.loadFromFile("Data/tails/fly-" + to_string(i) + ".png");
            flyRightTexture.update(img, i * 56, 0);
            animFlyRight.addFrame(i * 56, 0, 56, 48);
        }
        animFlyRight.setSprite(&sprite); animFlyRight.setLoop(true);
        flyLeftTexture = flyRightTexture;
        animFlyLeft = animFlyRight; animFlyLeft.setSprite(&sprite);

        // Starting animation
        sprite.setTexture(idleRightTexture, true);
        currentAnim = &animIdleRight;
        currentAnim->play();
    }

    // Load a static texture for construction
    static Texture& getDefaultTexture() {
        static Texture tex; static bool loaded = false;
        if (!loaded) { tex.loadFromFile("Data/tails/idle.png"); loaded = true; }
        return tex;
    }

    // Handle directional movement and flight toggle
    void handleInput() override {
        if (!controlled) {
            flying = false;
            prevFly = false;
            return;
        }

        bool left = Keyboard::isKeyPressed(Keyboard::Left);
        bool right = Keyboard::isKeyPressed(Keyboard::Right);

        isFacingRight = right || (!left && isFacingRight);

        if (left) velocityX = -speed;
        else if (right) velocityX = speed;
        else velocityX *= deceleration;

        if (Keyboard::isKeyPressed(Keyboard::Space))
            jump();

        bool fly = Keyboard::isKeyPressed(Keyboard::F) && controlled;

        // Start flight on rising edge
        if (fly && !prevFly && !flying) {
            flying = true;
            flightClock.restart();
        }
        // Stop flight when key released
        else if (!fly && flying) {
            flying = false;
        }

        prevFly = fly;
    }

    // Physics, collision, flight, and animation update
    void update(Grid& grid) override {
        box = onGround ? &groundBox : &airBox;

        const int pixels = grid.getPixels();
        const int W = grid.getWidth();
        const int H = grid.getHeight();

        // Horizontal movement
        float oldX = x;
        handleInput();
        x += velocityX;
        box->updateHitbox(x, y);
        if (box->collides(grid)) {
            x = oldX; velocityX = 0.f;
            box->updateHitbox(x, y);
        }

        // Vertical logic (gravity vs flight)
        if (flying) {
            velocityY = -4.f;
            if (flightClock.getElapsedTime().asSeconds() >= flightDuration) {
                flying = false;
                flightDuration = baseFlightSec;
            }
        }
        else {
            velocityY += gravity;
            if (velocityY > terminalVelocity)
                velocityY = terminalVelocity;
        }

        float oldY = y;
        y += velocityY;
        box->updateHitbox(x, y);

        // Landing check
        bool hitWall = box->collides(grid);
        bool hitPlat = (velocityY > 0.f) && box->collidePlatformFromAbove(grid);

        if (velocityY > 0.f && (hitWall || hitPlat)) {
            float newTop = int(box->getBottom() / pixels) * pixels - box->getHeight();
            float offY = box->getTop() - y;
            y = newTop - offY - 0.5f;
            velocityY = 0.f;
            onGround = true;
            flying = false;
            box->updateHitbox(x, y);
        }
        else {
            onGround = false;
        }

        // Keep player within screen bounds
        if (x < 0) x = 0;
        if (x > W * pixels - box->getWidth()) x = W * pixels - box->getWidth();
        if (y < 0) y = 0;

        // Update sprite position
        sprite.setPosition(x, y);

        // Choose correct animation
        if (flying) {
            animation* next = isFacingRight ? &animFlyRight : &animFlyLeft;
            if (currentAnim != next) {
                currentAnim = next;
                sprite.setTexture(isFacingRight ? flyRightTexture : flyLeftTexture, true);
                currentAnim->play();
            }
        }
        else if (!onGround) {
            animation* next = isFacingRight ? &animJumpRight : &animJumpLeft;
            if (currentAnim != next) {
                currentAnim = next;
                sprite.setTexture(isFacingRight ? jumpRightTexture : jumpLeftTexture, true);
                currentAnim->play();
            }
        }
        else if (abs(velocityX) > 0.5f) {
            animation* next = (velocityX > 0) ? &animRunRight : &animRunLeft;
            if (currentAnim != next) {
                currentAnim = next;
                sprite.setTexture((velocityX > 0) ? runRightTexture : runLeftTexture, true);
                currentAnim->play();
            }
        }
        else {
            animation* next = isFacingRight ? &animIdleRight : &animIdleLeft;
            if (currentAnim != next) {
                currentAnim = next;
                sprite.setTexture(isFacingRight ? idleRightTexture : idleLeftTexture, true);
                currentAnim->play();
            }
        }

        // Play current animation
        if (currentAnim) currentAnim->update();
    }

    // Adds 4s to next flight time
    void applyBoost() override {
        flightDuration += 4.f;
    }

    void updateBoost() override {}

    // AI & follow functions
    void setVelocityX(float v) override { velocityX = v; }
    float getMaxSpeed() const override { return speed; }
    bool isFlying() const { return flying; }
    bool isCarrier() const override { return flying; }
    void setDec(float f) override { deceleration = f; }
    void setSpeed(float f) override { speed -= f; }
};
