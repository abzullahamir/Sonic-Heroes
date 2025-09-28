#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace sf;

#include "scoreboard.h"
#include "menu.h"
#include "levels.h"
#include "runGame.h"
#include "LabyrinthZone.h"
#include "iceCapZone.h"
#include "deathEggZone.h"
#include "boss.h"

class Game {
private:
    static const int screen_x = 1200;
    static const int screen_y = 900;
    static const int numPlayers = 3;

    RenderWindow window;
    scoreboard   score;
    Menu         menu;
    Levels* currentLevel;
    bool         sessionOver;
    int          mode;
//0 for menu
//1 for game
// 2 for loading
// 3 for leaderboard
// 4 for options
// 5 for exit
bool         soundOn = true;
    int          optionSelected = 0;
    bool         levelClearedFlag = false;
    bool         quitFlag = false;
    int          currentLevelIndex = 1;
    static const int maxLevels = 4;

public:
    Game()
        : window(VideoMode(screen_x, screen_y), "Sonic Heroes", Style::Close)
        , currentLevel(nullptr)
        , menu(window.getSize().x, window.getSize().y)
        , mode(0)
    {
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
    }

    void run() {
        while (window.isOpen()) {
            // ── Main menu / input
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();

                if (event.type == Event::KeyReleased) {
                    switch (event.key.code) {
                    case Keyboard::Up:
                        menu.up();
                        break;
                    case Keyboard::Down:
                        menu.down();
                        break;
                    case Keyboard::Enter: {
                        int choice = menu.select();
                        switch (choice) {
                        case 0: mode = 1; break;              // Play
                        case 1: mode = 2; break;              // Continue
                        case 2: mode = 3; score.displayHighScores(window); break;
                        case 3:                                // Options
                            mode = 4;
                            optionSelected = 0;
                            break;
                        case 4:                                // Exit
                            window.close();
                            break;
                        }
                        break;
                    }
                    default: break;
                    }
                }
            }

            
            switch (mode) {
            case 0: {  // Main Menu
                window.clear(Color::Black);
                menu.draw(window);
                window.display();
                break;
            }

            case 1:    
            {
                selectLevel(currentLevelIndex);
                playLevel();

                if (quitFlag) {
                    // 1 User hit Esc 
                    mode = 0;
                }
                else if (levelClearedFlag) {
                    ++currentLevelIndex;
                    if (currentLevelIndex > maxLevels) {
                        // 2 All levels done 
                        showCongratsScreen();
                    }
                    else {
                        // Beat this level but more remain
                        mode = 0;
                    }
                }
                else {
                    // 3 Died = You lost screen
                    showLostScreen();
                }
                break;
            }


            case 2: {  // Continue
                playLevel();
                mode = 0;
                break;
            }

            case 3: {  // Leaderboard
                window.clear(Color::Black);
                score.displayHighScores(window);
                window.display();
                //  for Escape or Enter
                Event e;
                while (window.pollEvent(e)) {
                    if (e.type == Event::KeyReleased &&
                        (e.key.code == Keyboard::Enter ||
                            e.key.code == Keyboard::Escape))
                    {
                        mode = 0;
                    }
                    else if (e.type == Event::Closed) {
                        window.close();
                    }
                }
                break;
            }

            case 4: {  // Options
                window.clear(Color::Black);

                static Font optFont;
                static bool fontLoaded = false;
                if (!fontLoaded) {
                    optFont.loadFromFile("arial.ttf");
                    fontLoaded = true;
                }

                Text volTxt(soundOn ? "Sound: On" : "Sound: Off", optFont, 36);
                volTxt.setPosition(400, 300);
                volTxt.setFillColor(optionSelected == 0 ? Color::Red : Color::White);

                Text backTxt("Back", optFont, 36);
                backTxt.setPosition(400, 380);
                backTxt.setFillColor(optionSelected == 1 ? Color::Red : Color::White);

                window.draw(volTxt);
                window.draw(backTxt);
                window.display();

                Event e;
                while (window.pollEvent(e)) {
                    if (e.type == Event::KeyReleased) {
                        switch (e.key.code) {
                        case Keyboard::Up:
                            if (optionSelected > 0) optionSelected--;
                            break;
                        case Keyboard::Down:
                            if (optionSelected < 1) optionSelected++;
                            break;
                        case Keyboard::Left:
                        case Keyboard::Right:
                            if (optionSelected == 0) {
                                soundOn = !soundOn;
                            }
                            break;
                        case Keyboard::Enter:
                        case Keyboard::Escape:
                            if (optionSelected == 1 || e.key.code == Keyboard::Escape)
                                mode = 0;
                            break;
                        }
                    }
                    else if (e.type == Event::Closed) {
                        window.close();
                    }
                }
                break;
            }

            }

        } 
    }

    void playLevel() {
        runGame session(&window, currentLevel);
        session.initialize();
        session.run();
        score.addScore(session.getScore());
        levelClearedFlag = session.levelCleared();
        quitFlag = session.wasQuit();
    }

    void selectLevel(int levelIndex) {
        if (currentLevel) {
            delete currentLevel;
            currentLevel = nullptr;
        }

        if (levelIndex == 1) currentLevel = new LabyrinthZone();
        else if (levelIndex == 2) currentLevel = new iceCapZone();
        else if (levelIndex == 3) currentLevel = new deathEggZone();
        else if (levelIndex == 4) currentLevel = new BossZone();
        else if (levelIndex>=maxLevels){}
    }
    // Display “Congratulations” until Enter or Esc then back to menu
    void showCongratsScreen() {
        // prepare text
        Font f; f.loadFromFile("arial.ttf");
        Text msg("Congratulations!\nYou won!", f, 48);
        msg.setFillColor(Color::White);
        msg.setPosition(300, 350);

        // draw & wait
        window.clear(Color::Black);
        window.draw(msg);
        window.display();

        // wait for any key or close
        Event e;
        while (window.waitEvent(e)) {
            if (e.type == Event::KeyReleased ||
                e.type == Event::Closed)
            {
                mode = 0;    // back to main menu
                break;
            }
        }
    }

    void showLostScreen() {
        Font f; f.loadFromFile("arial.ttf");
        Text msg("You lost!\nTry again?", f, 48);
        msg.setFillColor(Color::White);
        msg.setPosition(400, 350);

        window.clear(Color::Black);
        window.draw(msg);
        window.display();

        Event e;
        while (window.waitEvent(e)) {
            if (e.type == Event::KeyReleased ||
                e.type == Event::Closed)
            {
                mode = 0;    // back to main menu
                break;
            }
        }
    }

};
