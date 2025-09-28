#pragma once
#include "player.h"
#include "animation.h"
#include "hitbox.h"
#include "grid.h"
#include <SFML/Audio.hpp>
#include <string>

using namespace sf;
using namespace std;

class Knuckles : public player {
private:
    float velocityX = 0.f;
    float speed = 12.f;
    float deceleration = 0.80f;

    bool isFacingRight = true;
    bool punching = false;

    Texture runRightTexture, runLeftTexture;
    Texture idleRightTexture, idleLeftTexture;
    Texture jumpRightTexture, jumpLeftTexture;
    Texture punchRightTexture, punchLeftTexture, hangTexture, hangMirrorTexture;

    animation animRunRight, animRunLeft;
    animation animIdleRight, animIdleLeft;
    animation animJumpRight, animJumpLeft;
    animation animPunchRight, animPunchLeft, animHang, animHangMirror;

    animation* currentAnim = nullptr;

    bool invincible = false;
    float invDuration = 0.f;
    Clock invClock;

    SoundBuffer punchBuf;
    Sound punchSfx;

public:
    Knuckles(float startX = 0, float startY = 100)
        : player(getDefaultTexture(), startX, startY, 2.5f, 2.5f)
    {
        float hbW = 20.5f * 2.5f;
        float hbH = 38.f * 2.5f;
        float offX = 33.f;
        float offY = 19.f;

        groundBox = hitBox(startX, startY, hbW, hbH, offX, offY);
        airBox = hitBox(startX, startY + 12.5f, 51.25f, 80.0f, 33.0f, 19.0f);
        box = &groundBox;

        runRightTexture.create(9 * 48, 48);
        for (int i = 0; i < 9; ++i) {
            Image img;
            img.loadFromFile("Data/knuckles/jog-" + to_string(i) + ".png");
            runRightTexture.update(img, i * 48, 0);
            animRunRight.addFrame(i * 48, 0, 48, 48);
        }
        animRunRight.setSprite(&sprite);
        animRunRight.setLoop(true);

        runLeftTexture = runRightTexture;
        animRunLeft = animRunRight;
        animRunLeft.setSprite(&sprite);

        idleRightTexture.loadFromFile("Data/knuckles/idel.png");
        animIdleRight.setSprite(&sprite);
        animIdleRight.addFrame(0, 0, 48, 48);
        animIdleRight.setLoop(true);

        idleLeftTexture.loadFromFile("Data/knuckles/idel.png");
        animIdleLeft.setSprite(&sprite);
        animIdleLeft.addFrame(0, 0, 48, 48);
        animIdleLeft.setLoop(true);

        jumpRightTexture.create(6 * 48, 48);
        for (int i = 0; i < 6; ++i) {
            Image img;
            img.loadFromFile("Data/knuckles/jump-" + to_string(i) + ".png");
            jumpRightTexture.update(img, i * 48, 0);
            animJumpRight.addFrame(i * 48, 0, 48, 48);
        }
        animJumpRight.setSprite(&sprite);

        jumpLeftTexture = jumpRightTexture;
        animJumpLeft = animJumpRight;
        animJumpLeft.setSprite(&sprite);

        punchRightTexture.loadFromFile("Data/knuckles/punch.png");
        sprite.setTexture(punchRightTexture);
        animPunchRight.setSprite(&sprite);
        for (int i = 0; i < 5; ++i) {
            animPunchRight.addFrame(i * 50, 0, 50, 40);
        }
        animPunchRight.setLoop(false);

        punchLeftTexture = punchRightTexture;
        animPunchLeft = animPunchRight;
        animPunchLeft.setSprite(&sprite);

        hangTexture.create(48, 48);
        {
            Image img;
            img.loadFromFile("Data/knuckles/hang-0.png");
            hangTexture.update(img, 0, 0);
        }
        animHang.setSprite(&sprite);
        animHang.addFrame(0, 0, 48, 48);
        animHang.setLoop(true);

        hangMirrorTexture.create(48, 48);
        {
            Image img;
            img.loadFromFile("Data/knuckles/hangMirror.png");
            hangMirrorTexture.update(img, 0, 0);
        }
        animHangMirror.setSprite(&sprite);
        animHangMirror.addFrame(0, 0, 48, 48);
        animHangMirror.setLoop(true);

        punchBuf.loadFromFile("Data/Destroy.wav");
        punchSfx.setBuffer(punchBuf);

        sprite.setTexture(idleRightTexture, true);
        currentAnim = &animIdleRight;
        currentAnim->play();
    }

    static Texture& getDefaultTexture() {
        static Texture tex;
        static bool loaded = false;
        if (!loaded) {
            tex.loadFromFile("Data/knuckles/idle-right.png");
            loaded = true;
        }
        return tex;
    }

    // input
    void handleInput() override {
        if (!controlled) return;

        bool left = Keyboard::isKeyPressed(Keyboard::Left);
        bool right = Keyboard::isKeyPressed(Keyboard::Right);
        isFacingRight = right || (!left && isFacingRight);

        // movement control
        if (left) velocityX = -speed;
        else if (right) velocityX = speed;
        else velocityX *= deceleration;

        // jump
        if (Keyboard::isKeyPressed(Keyboard::Space)) jump();

        // attack
        punching = Keyboard::isKeyPressed(Keyboard::Q) && controlled;
        if (punching) punchSfx.play();
    }

    // update
    void update(Grid& grid) override {
        // hanging animation logic
        if (isHanging()) {
            animation* next = isFacingRight ? &animHang : &animHangMirror;
            if (currentAnim != next) {
                currentAnim = next;
                sprite.setTexture(isFacingRight ? hangTexture : hangMirrorTexture, true);
                currentAnim->play();
            }
            currentAnim->update();
            sprite.setPosition(x, y);
            box->updateHitbox(x, y);
            return;
        }

        updateBoost(); // boost timer

        // choose hitbox
        box = onGround ? &groundBox : &airBox;

        int pixels = grid.getPixels();
        int levelW = grid.getWidth();
        int levelH = grid.getHeight();

        // horizontal movement
        float oldX = x;
        handleInput(); // get input
        x += velocityX;
        box->updateHitbox(x, y);
        if (box->collides(grid)) {
            x = oldX;
            velocityX = 0.f;
            box->updateHitbox(x, y);
        }

        // vertical movement
        velocityY += gravity;
        if (velocityY > terminalVelocity)
            velocityY = terminalVelocity;

        float oldY = y;
        y += velocityY;
        box->updateHitbox(x, y);

        // ground collision
        bool hitWall = box->collides(grid);
        bool hitPlatform = (velocityY > 0.f) && box->collidePlatformFromAbove(grid);

        if (velocityY > 0.f && (hitWall || hitPlatform)) {
            float hitboxBottom = box->getBottom();
            int hitRow = int(hitboxBottom / pixels);
            float newTop = hitRow * pixels - box->getHeight();
            float offY = box->getTop() - y;
            y = newTop - offY - 0.5f;
            velocityY = 0.f;
            onGround = true;
            box->updateHitbox(x, y);
        }
        else {
            onGround = false;
        }

        // world bounds
        if (x < 0) x = 0;
        if (x > levelW * pixels - box->getWidth())
            x = levelW * pixels - box->getWidth();
        if (y < 0) y = 0;

        sprite.setPosition(x, y);

        // attack event - break wall
        if (punching) {
            int baseCol = int((isFacingRight ? box->getRight() : box->getLeft()) / pixels);
            int targetCol = baseCol + (isFacingRight ? 1 : -1);
            int midRow = int(((box->getTop() + box->getBottom()) * 0.5f) / pixels);

            if (grid.getTile(targetCol, midRow) == 'k')
                grid.setTile(targetCol, midRow, ' ');

            int aboveRow = midRow - 1;
            if (aboveRow >= 0 && grid.getTile(targetCol, aboveRow) == 'k')
                grid.setTile(targetCol, aboveRow, ' ');
        }

        // animation update
        if (punching) {
            animation* next = isFacingRight ? &animPunchRight : &animPunchLeft;
            if (currentAnim != next) {
                currentAnim = next;
                sprite.setTexture(isFacingRight ? punchRightTexture : punchLeftTexture, true);
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

        // flicker when invincible
        if (invincible) {
            float t = invClock.getElapsedTime().asSeconds();
            sprite.setColor((int(t * 10) % 2) ? Color::Transparent : Color::White);
        }
        else {
            sprite.setColor(Color::White);
        }

        if (currentAnim)
            currentAnim->update();
    }

    // boost event
    void applyBoost() override {
        invincible = true;
        invDuration = 15.f;
        invClock.restart();
    }

    void updateBoost() override {
        if (invincible &&
            invClock.getElapsedTime().asSeconds() >= invDuration) {
            invincible = false;
        }
    }

    // AI movement
    void setVelocityX(float v) override { velocityX = v; }
    float getMaxSpeed() const override { return speed; }
    void setDec(float f) override { deceleration = f; }
    void setSpeed(float f) override { speed -= f; }
};
