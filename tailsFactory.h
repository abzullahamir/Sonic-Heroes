#pragma once
#include "playerFactory.h"
#include "tails.h"

class tailsFactory :public playerFactory {
public:
    tailsFactory() {
    }
    player* createPlayer() override {
        myPlayer = new Tails();
        return myPlayer;
    }
    ~tailsFactory() override = default;
};;
