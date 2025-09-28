
#pragma once
#include "collectibleFactory.h"
#include "extraLife.h"

class ExtraLifeFactory : public CollectibleFactory {
public:
	collectibles* create() override { return new ExtraLife(); }
	char symbol()    const override { return 'l'; }
};
