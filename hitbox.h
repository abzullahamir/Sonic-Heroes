#pragma once
#include "grid.h"
#include <cmath>
using namespace std;
class hitBox {
private:
	// private members
	// hitbox_x, hitbox_y are the top left corner of the hitbox
	// hitbox_width, hitbox_height are the width and height of the hitbox
	// offsetX, offsetY are the offsets from the sprite position
	float hitbox_x,
		  hitbox_y,
		  hitbox_width,
		  hitbox_height,
		  offsetX,
		  offsetY;
public:
	
	hitBox(float x = 0, float y = 0,
		float w = 0, float h = 0,
		float oX = 0, float oY = 0) :
		hitbox_x(x), hitbox_y(y), hitbox_width(w),
		hitbox_height(h), offsetX(oX),
		offsetY(oY) {
	
	}

	void updateHitbox(float spriteX, float spriteY) {
		hitbox_x = spriteX + offsetX;
		hitbox_y = spriteY + offsetY;
	}

	bool intersects(const hitBox& hit) const{
		if (getRight() < hit.getLeft() ||
			getLeft() > hit.getRight() ||
			getTop() > hit.getBottom() ||
			getBottom() < hit.getTop())
			return false;

		else return true;
	}

	/**
 * Check collision against solid floor tiles ('w') using 8 sample points.
 */
	bool collides(const Grid& grid) {
		int cellSize = grid.getPixels();

		float left = hitbox_x;
		float right = hitbox_x + hitbox_width;
		float top = hitbox_y;
		float bottom = hitbox_y + hitbox_height;
		float midX = (left + right) * 0.5f;
		float midY = (top + bottom) * 0.5f;

		// 8 sample points: corners + edge-midpoints
		float sampleX[8] = {
			left,  midX,   right,
			left,  right,
			left,  midX,   right
		};
		float sampleY[8] = {
			top,   top,    top,
			midY,  midY,
			bottom,bottom, bottom
		};

		for (int i = 0; i < 8; ++i) {
			int col = int(sampleX[i] / cellSize);
			int row = int(sampleY[i] / cellSize);
			if (grid.getTile(col, row) == 'w' || (grid.getTile(col, row) == 'k')) 
				return true;
		}
		return false;
	}
	bool collidePlatformFromAbove(const Grid& grid) const {
		int cellSize = grid.getPixels();

		// Row just below the bottom edge of this hitbox
		int bottomRow = int((hitbox_y + hitbox_height) / cellSize);

		// Columns spanning the hitbox
		int leftCol = int(hitbox_x / cellSize);
		int rightCol = int((hitbox_x + hitbox_width - 1) / cellSize);

		// If any tile underfoot is 'p', report a collision
		for (int col = leftCol; col <= rightCol; ++col) {
			if (grid.getTile(col, bottomRow) == 'p')
				return true;
		}
		return false;
	}
	


	float getLeft() const
	{
		return hitbox_x;
	}
	float getRight() const 
	{
		return hitbox_x + hitbox_width;
	}
	float getTop() const 
	{
		return hitbox_y;
	}
	float getBottom() const
	{
		return hitbox_y + hitbox_height;
	}
	float getHeight() const {
		return hitbox_height;
	}
	float getWidth() const {
		return hitbox_width;
	}

};