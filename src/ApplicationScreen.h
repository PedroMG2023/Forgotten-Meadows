// ApplicationScreen.h
#pragma once

#include "Screen.h"

class ApplicationScreen : public Screen {
public:
    int run(sf::RenderWindow& window) override {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                return -1;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                return 0;
            }
        }

        window.clear();
        window.display();

        return 1;
    }
};
