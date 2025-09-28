#pragma once
#include "player.h"
#include "animation.h"
#include "hitbox.h"
#include "grid.h"
#include "string"

class Sonic : public player {
private:
    float velocityX = 0.f;
    float acceleration;
    float deceleration;
    bool isFacingRight = true;
    float baseSpeed = 18.f;

    bool boostActive = false;
    Clock boostTimer;
    float boostDuration = 0.f;
    const float speedBonus = 4.f;

    Texture runRightTexture, hangTexture, hangMirrorTexture, runLeftTexture;
    Texture idleRightTexture, idleLeftTexture;
    Texture jumpTexture, leftJumpTexture;
    Texture runFastRightTexture, runFastLeftTexture;

    animation animRunRight, animRunLeft, animHang, animHangMirror;
    animation animIdleRight, animIdleLeft;
    animation animJump, animleftJump;
    animation animRunFastRight, animRunFastLeft;

    animation* currentAnim = nullptr;

public:
    Sonic(float startX = 0, float startY = 100)
        : player(getDefaultTexture(), startX, startY, 2.5f, 2.5f),
        velocityX(0.f), acceleration(0.2f), deceleration(0.80f)
    {
        // load run right animation
        runRightTexture.create(11 * 40, 40);
        for (int i = 0; i < 11; ++i) {
            Image img; img.loadFromFile("Data/sonic/0right-" + to_string(i) + ".png");
            runRightTexture.update(img, i * 40, 0);
            animRunRight.addFrame(i * 40, 0, 40, 40);
        }
        animRunRight.setSprite(&sprite);
        animRunRight.setLoop(true);

        // load run left animation
        runLeftTexture.create(11 * 40, 40);
        for (int i = 0; i < 11; ++i) {
            Image img; img.loadFromFile("Data/sonic/0left-" + to_string(i) + ".png");
            runLeftTexture.update(img, i * 40, 0);
            animRunLeft.addFrame(i * 40, 0, 40, 40);
        }
        animRunLeft.setSprite(&sprite);
        animRunLeft.setLoop(true);

        // fast run right animation
        runFastRightTexture.create(8 * 40, 40);
        for (int i = 0; i < 8; ++i) {
            Image img; img.loadFromFile("Data/sonic/0right_run-" + to_string(i) + ".png");
            runFastRightTexture.update(img, i * 40, 0);
            animRunFastRight.addFrame(i * 40, 0, 40, 40);
        }
        animRunFastRight.setSprite(&sprite);
        animRunFastRight.setLoop(true);

        // fast run left animation
        runFastLeftTexture.create(8 * 40, 40);
        for (int i = 0; i < 8; ++i) {
            Image img; img.loadFromFile("Data/sonic/0left_run-" + to_string(i) + ".png");
            runFastLeftTexture.update(img, i * 40, 0);
            animRunFastLeft.addFrame(i * 40, 0, 40, 40);
        }
        animRunFastLeft.setSprite(&sprite);
        animRunFastLeft.setLoop(true);

        // idle right
        idleRightTexture.loadFromFile("Data/0right_still.png");
        animIdleRight.setSprite(&sprite);
        animIdleRight.addFrame(0, 0, 40, 40);
        animIdleRight.setLoop(true);

        // idle left
        idleLeftTexture.loadFromFile("Data/0left_still.png");
        animIdleLeft.setSprite(&sprite);
        animIdleLeft.addFrame(0, 0, 40, 40);
        animIdleLeft.setLoop(true);

        // jump right
        jumpTexture.create(7 * 40, 40);
        for (int i = 0; i < 7; ++i) {
            Image img; img.loadFromFile("Data/sonic/0upR-" + to_string(i) + ".png");
            jumpTexture.update(img, i * 40, 0);
            animJump.addFrame(i * 40, 0, 40, 40);
        }
        animJump.setSprite(&sprite);
        animJump.setLoop(false);

        // jump left
        leftJumpTexture.create(7 * 40, 40);
        for (int i = 0; i < 7; ++i) {
            Image img; img.loadFromFile("Data/sonic/0upL-" + to_string(i) + ".png");
            leftJumpTexture.update(img, i * 40, 0);
            animleftJump.addFrame(i * 40, 0, 40, 40);
        }
        animleftJump.setSprite(&sprite);
        animleftJump.setLoop(false);

        // hanging frames
        hangTexture.create(44, 44);
        {
            Image img;
            img.loadFromFile("Data/sonic/hang-0.png");
            hangTexture.update(img, 0, 0);
        }
        animHang.setSprite(&sprite);
        animHang.addFrame(0, 0, 44, 44);
        animHang.setLoop(true);

        hangMirrorTexture.create(44, 44);
        {
            Image img;
            img.loadFromFile("Data/sonic/hangMirror.png");
            hangMirrorTexture.update(img, 0, 0);
        }
        animHangMirror.setSprite(&sprite);
        animHangMirror.addFrame(0, 0, 44, 44);
        animHangMirror.setLoop(true);

        // set starting animation
        sprite.setTexture(idleRightTexture, true);
        currentAnim = &animIdleRight;
        currentAnim->play();
    }

    static Texture& getDefaultTexture() {
        static Texture tex;
        static bool loaded = false;
        if (!loaded) {
            tex.loadFromFile("Data/0left_still.png");
            loaded = true;
        }
        return tex;
    }

    // input handling with acceleration
    void handleInput() override {
        if (!controlled) return;

        bool left = Keyboard::isKeyPressed(Keyboard::Left);
        bool right = Keyboard::isKeyPressed(Keyboard::Right);

        if (left) isFacingRight = false;
        if (right) isFacingRight = true;

        float currentSpeed = boostActive ? baseSpeed + speedBonus : baseSpeed;
        float target = 0.f;
        if (left) target = -currentSpeed;
        if (right) target = currentSpeed;

        velocityX += (target - velocityX) * acceleration;

        if (Keyboard::isKeyPressed(Keyboard::Space)) jump();
    }

    // update movement, collisions, animation
    void update(Grid& grid) override {
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

        updateBoost();

        box = onGround ? &groundBox : &airBox;
        const int pixels = grid.getPixels();
        const int levelW = grid.getWidth();
        const int levelH = grid.getHeight();

        float oldX = x;
        handleInput();
        x += velocityX;
        box->updateHitbox(x, y);
        if (box->collides(grid)) {
            x = oldX;
            velocityX = 0.f;
            box->updateHitbox(x, y);
        }

        // apply friction
        if (!Keyboard::isKeyPressed(Keyboard::Left) &&
            !Keyboard::isKeyPressed(Keyboard::Right))
            velocityX *= deceleration;

        // vertical movement
        velocityY += gravity;
        if (velocityY > terminalVelocity)
            velocityY = terminalVelocity;

        float oldY = y;
        y += velocityY;
        box->updateHitbox(x, y);

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
        if (x > levelW * pixels - box->getWidth()) x = levelW * pixels - box->getWidth();
        if (y < 0) y = 0;

        sprite.setPosition(x, y);

        // animation state logic
        if (!onGround) {
            if (velocityX < -0.1f) {
                if (currentAnim != &animleftJump) {
                    currentAnim = &animleftJump;
                    sprite.setTexture(leftJumpTexture, true);
                    currentAnim->play();
                }
            }
            else {
                if (currentAnim != &animJump) {
                    currentAnim = &animJump;
                    sprite.setTexture(jumpTexture, true);
                    currentAnim->play();
                }
            }
        }
        else if (velocityX > 0.1f) {
            animation* nextAnim = boostActive ? &animRunFastRight : &animRunRight;
            Texture& nextTex = boostActive ? runFastRightTexture : runRightTexture;
            if (currentAnim != nextAnim) {
                currentAnim = nextAnim;
                sprite.setTexture(nextTex, true);
                currentAnim->play();
            }
        }
        else if (velocityX < -0.1f) {
            animation* nextAnim = boostActive ? &animRunFastLeft : &animRunLeft;
            Texture& nextTex = boostActive ? runFastLeftTexture : runLeftTexture;
            if (currentAnim != nextAnim) {
                currentAnim = nextAnim;
                sprite.setTexture(nextTex, true);
                currentAnim->play();
            }
        }
        else {
            if (isFacingRight) {
                if (currentAnim != &animIdleRight) {
                    currentAnim = &animIdleRight;
                    sprite.setTexture(idleRightTexture, true);
                    currentAnim->play();
                }
            }
            else {
                if (currentAnim != &animIdleLeft) {
                    currentAnim = &animIdleLeft;
                    sprite.setTexture(idleLeftTexture, true);
                    currentAnim->play();
                }
            }
        }

        if (currentAnim)
            currentAnim->update();
    }

    // activate speed boost
    void applyBoost() override {
        boostActive = true;
        boostDuration = 4.f;
        boostTimer.restart();
    }

    // check if boost expired
    void updateBoost() override {
        if (boostActive && boostTimer.getElapsedTime().asSeconds() >= boostDuration)
            boostActive = false;
    }

    // AI hooks
    void setVelocityX(float v) override { velocityX = v; }
    float getMaxSpeed() const override {
        return boostActive ? (baseSpeed + speedBonus) : baseSpeed;
    }
    void setDec(float f) override { deceleration = f; }
    void setSpeed(float f) override { baseSpeed -= f; }
};
