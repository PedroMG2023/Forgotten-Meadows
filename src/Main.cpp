// main.cpp
#include <SFML/Graphics.hpp>
#include "MenuScreen.h"
#include "ApplicationScreen.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Screens");

    std::vector<Screen*> screens;
    screens.push_back(new MenuScreen());
    screens.push_back(new ApplicationScreen());

    int screenIndex = 0;

    while (screenIndex >= 0) {
        screenIndex = screens[screenIndex]->run(window);
    }

    for (auto screen : screens) {
        delete screen;
    }

    return 0;
}