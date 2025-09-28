#pragma once
#include <SFML/Graphics.hpp>
#include "hitbox.h"
#include "grid.h"
using namespace sf;

class player {
protected:
    Sprite  sprite;

    float speed;
    float gravity;
    float terminalVelocity;
    float velocityY;
    float jumpStrength;
    bool  onGround;
    bool controlled = true;

    hitBox groundBox;
    hitBox airBox;
    hitBox* box;

    bool movingLeft, movingRight;

    float x, y;
    bool hanging = false;

    SoundBuffer jumpBuf;
    Sound       jumpSfx;

public:
    player(Texture& texture,
        float startX,
        float startY,
        float scaleX = 1.f,
        float scaleY = 1.f)
        : sprite(texture),
        groundBox(startX, startY, 24 * scaleX, 36 * scaleY, 8 * scaleX, 2.5f * scaleY),
        airBox(startX, startY + 5.f * scaleY, 24.f * scaleX, 30.f * scaleY, 8.f * scaleX, 2.5f * scaleY),
        box(&groundBox),
        speed(5.f), gravity(1.f), terminalVelocity(20.f),
        velocityY(0.f), jumpStrength(-20.f), onGround(false),
        movingLeft(false), movingRight(false),
        x(startX), y(startY)
    {
        sprite.setScale(scaleX, scaleY);
        sprite.setPosition(x, y);
        if (!jumpBuf.loadFromFile("Data/jump.wav")) {
            std::cerr << "Failed to load Data/sounds/jump.wav\n";
        }
        jumpSfx.setBuffer(jumpBuf);
    }

    // input
    virtual void handleInput() {
        if (!controlled) return;

        movingLeft = Keyboard::isKeyPressed(Keyboard::Left);
        movingRight = Keyboard::isKeyPressed(Keyboard::Right);

        if (movingLeft)  x -= speed;
        if (movingRight) x += speed;

        if (Keyboard::isKeyPressed(Keyboard::Space))
            jump();
    }

    // jump logic
    virtual void jump() {
        if (onGround) {
            velocityY = jumpStrength;
            onGround = false;
            jumpSfx.play();
        }
    }

    // update player position and collisions
    virtual void update(Grid& grid) {
        box = onGround ? &groundBox : &airBox;

        const int pixels = grid.getPixels();
        const int levelW = grid.getWidth();
        const int levelH = grid.getHeight();

        // horizontal move and collision
        float oldX = x;
        handleInput();
        box->updateHitbox(x, y);
        if (box->collides(grid)) {
            x = oldX;
            box->updateHitbox(x, y);
        }

        // applying gravity
        velocityY += gravity;
        if (velocityY > terminalVelocity)
            velocityY = terminalVelocity;

        y += velocityY;
        box->updateHitbox(x, y);

        // checking floor/platform collision
        bool hitFloor = box->collides(grid);
        bool hitPlatform = (velocityY > 0.f) && box->collidePlatformFromAbove(grid);
        if (velocityY > 0.f && (hitFloor || hitPlatform)) {
            float bottomY = y + box->getHeight();
            int hitRow = int(bottomY / pixels);
            y = hitRow * pixels - box->getHeight();
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
        if (y > levelH * pixels - box->getHeight())
            y = levelH * pixels - box->getHeight();

        // final update
        box->updateHitbox(x, y);
        sprite.setPosition(x, y);
    }

    // drawing player sprite and sample hit points
    virtual void draw(RenderWindow& win, float offsetX) {
        float origX = sprite.getPosition().x;
        float origY = sprite.getPosition().y;
        sprite.setPosition(origX - offsetX, origY);
        win.draw(sprite);
        sprite.setPosition(origX, origY);

        float left = box->getLeft();
        float right = box->getRight();
        float top = box->getTop();
        float bottom = box->getBottom();
        float midX = (left + right) * 0.5f;
        float midY = (top + bottom) * 0.5f;

        float sampleX[8] = { left,  midX,   right,
                             left,  right,
                             left,  midX,   right };
        float sampleY[8] = { top,   top,    top,
                             midY,  midY,
                             bottom,bottom, bottom };

        CircleShape dot(2.f);
        dot.setFillColor(Color::Green);

        for (int i = 0; i < 8; ++i) {
            dot.setPosition(sampleX[i] - 2.f - offsetX, sampleY[i] - 2.f);
            win.draw(dot);
        }
    }

    void setControlled(bool flag) { controlled = flag; }
    bool isControlled() const { return controlled; }

    virtual void setVelocityX(float) {}
    virtual float getMaxSpeed() const { return speed; }

    bool isOnGround()  const { return onGround; }
    float getWidth()   const { return box->getWidth(); }
    float getHeight()  const { return box->getHeight(); }

    void setX(float newX) {
        x = newX;
        box->updateHitbox(x, y);
    }

    void setY(float newY) {
        y = newY;
        box->updateHitbox(x, y);
    }

    float getX() const { return x; }
    float getY() const { return y; }
    hitBox& getHitbox() { return *box; }

    // boost logic
    virtual void applyBoost() = 0;
    virtual void updateBoost() {}

    void setHanging(bool h) { hanging = h; }
    bool isHanging() const { return hanging; }
    virtual bool isCarrier() const { return false; }

    bool ismovingRight() { return movingRight; }

    bool isRolling() const { return !onGround; }

    void setColor(const Color& c) { sprite.setColor(c); }

    virtual void setDec(float f) = 0;
    virtual void setGrav(float f) { gravity -= f; }
    virtual void setSpeed(float f) = 0;

    float getVelY() const { return velocityY; }
};
