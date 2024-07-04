#pragma once
#include "SFML/Graphics.hpp"
#define MAX_NUMBER_OF_ITEMS 3

enum MenuState { MAIN_MENU, OPTIONS_MENU };

#define MAX_NUMBER_OF_OPTIONS_ITEMS 3

class Menu
{
public:
    Menu(float width, float height);
    ~Menu();

    void draw(sf::RenderWindow& window);
    void MoveUp();
    void MoveDown();
    bool loadResources();
    void changeState(MenuState newState);
    void toggle(int a);
    bool isActive[2];


    MenuState getCurrentState() { return currentState; }

    //Get the window width and height
    float getWindowWidth() { return rectangulo.getSize().x; }

    int GetPressedItem() { return selectedItemIndex; }

private:

    MenuState currentState;
    sf::Text optionsMenu[MAX_NUMBER_OF_OPTIONS_ITEMS];

    int selectedItemIndex;

    sf::ConvexShape selectorTriangle;
    sf::RectangleShape rectangulo;
    sf::Text Title;
    sf::Font font;
    sf::Font titleFont;
    sf::Text menu[MAX_NUMBER_OF_ITEMS];
    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Vector2u textureSize;
    sf::RectangleShape toggleShape[2];
    sf::CircleShape circleShape[2];
    float triangleY;
    float scaleX;
    float scaleY;
};