// MenuScreen.h
#pragma once

#include "Screen.h"
#include <SFML/Graphics.hpp>;
#include "Menu.h"


class MenuScreen : public Screen {
public:
    int run(sf::RenderWindow& window) override {

        //Menu menu(window.getSize().x, window.getSize().y);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return -1;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G) {
                return -1;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                return 1;
            }
        }

        window.clear();
        
        window.display();

        return 0;
    }
};
