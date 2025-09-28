#pragma once
#include <iostream>
using namespace std;
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;

class Grid {
private:
	int height;
	int width;
	int pixels = 64;
	char** levelMap;

public:
	Grid(int h, int w) : height(h), width(w) {
		levelMap = new char* [height];
		for (int i = 0; i < height; i++) {
			levelMap[i] = new char[width];
			for (int j = 0; j < width; j++) {
				levelMap[i][j] = ' ';
			}
		}
	}
	~Grid() {
		for (int i = 0; i < height; ++i)
			delete[] levelMap[i];
		delete[] levelMap;
	}

	void loadFromFile(const string& filename) {
		ifstream fin(filename);
		if (!fin) {
			cerr << "Failed to open: " << filename << endl;
			return;
		}
		string line;
		for (int y = 0; y < height && getline(fin, line); ++y) {
			// make sure line.length() >= width, or pad with spaces
			for (int x = 0; x < width; ++x) {
				levelMap[y][x] = (x < (int)line.size() ? line[x] : ' ');
			}
		}
	}


	void setTile(int x, int y, char value) {
		
		if (x >= 0 && x < width && y >= 0 && y < height)
			levelMap[y][x] = value;
	}
	char getTile(int x, int y) const {
		// Out of bounds check
		if (x < 0 || x >= width || y < 0 || y >= height)
			return ' ';
		return levelMap[y][x];
	}
	void render(RenderWindow& window,
		float offsetX,
		Sprite & wallSprite,
		Sprite & breakableSprite,
		Sprite & springSprite,
		Sprite & crystalSprite,
		Sprite & platformSprite,
		Sprite & spikeSprite) const
	{
		const float scroll = offsetX;
		unsigned winW = 1200;

		for (int row = 0; row < height; ++row) {
			for (int col = 0; col < width; ++col) {
				char tile = levelMap[row][col];
				if (tile == ' ')
					continue;

				// world coords
				float worldX = col * pixels;
				float worldY = row * pixels;
				// screen coords after camera scroll
				float screenX = worldX - scroll;

				// horizontal culling
				if (screenX + pixels < 0 || screenX > winW)
					continue;

				Sprite* toDraw = nullptr;
				switch (tile) {
				case 'w': toDraw = &wallSprite;      break;
				case 'k': toDraw = &breakableSprite; break;
				case 'z': toDraw = &springSprite;    break;
				case 'c': toDraw = &crystalSprite;   break;
				case 'p': toDraw = &platformSprite;  break;
				case 'x': toDraw = &spikeSprite;     break;
				default:  break;
				}

				if (toDraw) {
					toDraw->setPosition(screenX, worldY);
					window.draw(*toDraw);
				}
			}
		}
	}
	int getHeight() const
	{
		return height;
	}
	int getWidth() const
	{
		return width;
	}
	int getPixels() const
	{
		return pixels;
	}
	// check if tile is solid at world coordinates
	bool isSolidAtWorld(float wx, float wy) const
	{
		// convert to column/row
		int col = static_cast<int>(wx / pixels);
		int row = static_cast<int>(wy / pixels);

		// out-of-bounds = no ground
		if (col < 0 || col >= getWidth() ||
			row < 0 || row >= getHeight())
			return false;

		// non-space tile = solid
		return (levelMap[row][col] != ' ');
	}

};