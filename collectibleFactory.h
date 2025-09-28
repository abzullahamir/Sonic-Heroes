#pragma once
#include "collectibles.h"

class CollectibleFactory {
public:
	virtual collectibles* create() = 0;
	virtual char symbol() const = 0;    // e.g. 'r' or 'l'
	virtual ~CollectibleFactory() {}
};
