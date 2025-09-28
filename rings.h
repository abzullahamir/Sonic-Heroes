#pragma once
#include "collectibles.h"

class Ring : public collectibles {
private:

public:
    Ring() {
        const int frameW = 16, frameH = 16, frameCount = 4;

        tex.create(frameCount * frameW, frameH);
        for (int i = 0; i < frameCount; ++i) {
            Image img;
            img.loadFromFile("Data/ring/ring-" + to_string(i) + ".png");
            tex.update(img, i * frameW, 0);
        }
        sprite.setTexture(tex);
        sprite.setScale(2.f, 2.f);
        anim.setSprite(&sprite);
        for (int i = 0; i < frameCount; ++i)
            anim.addFrame(i * frameW, 0, frameW, frameH);
        anim.setLoop(true);
        anim.setFrameDelay(4);       
        anim.play();
        box = hitBox(0, 0, frameW, frameH, 0, 0);
       
    }
    //implemented in rungame
    void onCollect(player& p) override { 
    }
    int getPointValue() const override { return 10; }

};
