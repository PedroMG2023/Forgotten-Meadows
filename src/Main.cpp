// main.cpp
#include <SFML/Graphics.hpp>
#include "MenuScreen.h"
#include "ApplicationScreen.h"
#include "Context.h"
int main() {
   
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktopMode, "Forgotten-Meadows", sf::Style::Fullscreen, sf::ContextSettings(24));
    window.setFramerateLimit(60);
    window.setActive(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);


    std::vector<Screen*> screens;
    screens.push_back(new MenuScreen(desktopMode.width, desktopMode.height));
    screens.push_back(new ApplicationScreen(window));

    int screenIndex = 0;

    Context context;
    context.Freecam = false;
    context.SaveStates = false;
    context.Sunset = false;


    while (screenIndex >= 0) {
        screenIndex = screens[screenIndex]->run(window, context);
        
    }

    for (auto screen : screens) {
        delete screen;
    }

    return 0;
}