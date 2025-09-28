#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
class animation {
private:
    Sprite* targetSprite;          // sprite we will change
    IntRect frames[32];      // up to 32 frames
    int frameCount;          // how many frames were added
    int current;             // current frame index
    int frameDelay;          // ticks to wait before switch
    int counter;             // counts those ticks
    bool playing;            // true while running
    bool loop;               // true = restart after last frame

public:
    animation()
        : targetSprite(nullptr), frameCount(0), current(0),
        frameDelay(6), counter(0), playing(false), loop(true)
    {
    }

    void setSprite(Sprite* spr) { targetSprite = spr; }

    // add one frame from the sheet
    void addFrame(int left, int top, int width, int height) {
        if (frameCount < 32) {
            frames[frameCount++] = IntRect(left, top, width, height);
        }
    }

    void setFrameDelay(int d) { frameDelay = d; }
    void setLoop(bool l) { loop = l; }

    // start playing 
    void play(bool restart = true) {
        if (frameCount == 0 || !targetSprite) return;
        if (restart) current = 0;
        playing = true;
        counter = 0;
        targetSprite->setTextureRect(frames[current]);
    }

    void stop() { playing = false; }

    // call once every game tick (e.g., 60 times a second)
    void update() {
        if (!playing || frameCount == 0 || !targetSprite) return;

        ++counter;
        if (counter >= frameDelay) {
            counter = 0;
            ++current;

            if (current >= frameCount) {          // reached end
                if (loop) current = 0;            // restart
                else {                            
                    current = frameCount - 1;
                    playing = false;
                }
            }
            targetSprite->setTextureRect(frames[current]);
        }
    }
};

