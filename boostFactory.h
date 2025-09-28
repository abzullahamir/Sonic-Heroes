#pragma once
#include "collectibleFactory.h"
#include "boost.h"

class boostFactory : public CollectibleFactory {
public:
	collectibles* create() override { return new boost(); }
	char symbol()    const override { return 'b'; }
};
