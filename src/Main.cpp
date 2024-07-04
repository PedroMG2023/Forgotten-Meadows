// main.cpp
#include <SFML/Graphics.hpp>
#include "MenuScreen.h"
#include "ApplicationScreen.h"

int main() {
   
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, ".", sf::Style::Fullscreen);

    std::vector<Screen*> screens;
    screens.push_back(new MenuScreen(desktopMode.width, desktopMode.height));
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