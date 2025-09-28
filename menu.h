//menu.h

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
using namespace sf;

class Menu {
private:
    int    selected;
    Font   font;
    Text* menuItems;

public:
    Menu(float width, float height)
        : selected(0)
    {
        menuItems = new Text[5];

        if (!font.loadFromFile("arial.ttf")) {}


        menuItems[0].setFont(font);
        menuItems[0].setFillColor(Color::Red);
        menuItems[0].setString("Play");
        menuItems[0].setPosition(width / 2, height / (5 + 1) * 1);

        // Item 2
        menuItems[1].setFont(font);
        menuItems[1].setFillColor(Color::White);
        menuItems[1].setString("Continue");
        menuItems[1].setPosition(width / 2, height / (5 + 1) * 2);

        // Item 3
        menuItems[2].setFont(font);
        menuItems[2].setFillColor(Color::White);
        menuItems[2].setString("Leaderboard");
        menuItems[2].setPosition(width / 2, height / (5 + 1) * 3);

        // Item 4
        menuItems[3].setFont(font);
        menuItems[3].setFillColor(Color::White);
        menuItems[3].setString("Options");
        menuItems[3].setPosition(width / 2, height / (5 + 1) * 4);

        // Item 5
        menuItems[4].setFont(font);
        menuItems[4].setFillColor(Color::White);
        menuItems[4].setString("Exit");
        menuItems[4].setPosition(width / 2, height / (5 + 1) * 5);
    }

    ~Menu() {
        delete[] menuItems;
    }

    void draw(RenderWindow& window) {
        for (int i = 0; i < 5; ++i)
            window.draw(menuItems[i]);
    }

    void up() {
        if (selected - 1 >= 0) {
            menuItems[selected--].setFillColor(Color::White);
            menuItems[selected].setFillColor(Color::Red);
        }
    }

    void down() {
        if (selected + 1 < 5) {
            menuItems[selected].setFillColor(Color::White);
            selected = (selected + 1) % 5;
            menuItems[selected].setFillColor(Color::Red);
        }
    }

    int select() const {
        return selected;
    }
};