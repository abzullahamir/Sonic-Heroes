#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
using namespace std;
using namespace sf;


class scoreboard {
private:
	int* highScores;
	int maxViewable; //max number of highscores displayed at one time
	Font font;
public:
	scoreboard():maxViewable(5) {
		highScores = new int[maxViewable];
		for (int i = 0; i < maxViewable; i++) {
			highScores[i] = 0;
		}
		loadHighScores();
		if (!font.loadFromFile("arial.ttf")) {}
	
	}
	void loadHighScores() {
		ifstream file("highscores.txt");
		if (file.is_open()) {
			for (int i = 0; i < maxViewable; i++) {
				file >> highScores[i];
			}
			file.close();
		}
	}
	void saveHighScores() {
		ofstream file("highscores.txt");
		if (file.is_open()) {
			for (int i = 0; i < maxViewable; i++) {
				file << highScores[i] << endl;
			}
			file.close();
		}
	}
	void addScore(int newScore){
		int index;
		// finds which score is it bigger than
		for (int i = 0;i < maxViewable;i++) {
			if (highScores[i] < newScore) {
				index = i;
				// replaces all the indexes ahead
				// adds the new one if yes
				for (int j = maxViewable - 1; j > index; --j) {
					highScores[j] = highScores[j - 1];
				}
				highScores[index] = newScore;

				break;
			}
		}
		//update in the file
		saveHighScores();
	}
	void displayHighScores(RenderWindow& window) {
		Text text;
		text.setFont(font);
		text.setCharacterSize(24);
		text.setFillColor(Color::White);

		for (int i = 0; i < maxViewable; i++) {
			text.setString(to_string(i + 1) + ". " + to_string(highScores[i]));
			text.setPosition(550, 150 + i * 30);
			window.draw(text);
		}

	}
	~scoreboard() {
		delete[] highScores;
	}
};
