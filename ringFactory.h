
#pragma once
#include "collectibleFactory.h"
#include "rings.h"

class RingFactory : public CollectibleFactory {
public:
	collectibles* create() override { return new Ring(); }
	char symbol()    const override { return 'r'; }
};
