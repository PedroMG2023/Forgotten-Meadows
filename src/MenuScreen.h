// MenuScreen.h
#pragma once

#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
//#include "Menu.h"
#include <filesystem.h>
#include <iostream>
#include "Context.h"

#define MAX_NUMBER_OF_ITEMS 3

enum MenuState { MAIN_MENU, OPTIONS_MENU };

#define MAX_NUMBER_OF_OPTIONS_ITEMS 3

class MenuScreen : public Screen {
public:

	bool isActive[2];
	MenuState currentState;
	MenuState getCurrentState() { return currentState; }
	

	//Get the window width and height
	float getWindowWidth() { return rectangulo.getSize().x; }

	int GetPressedItem() { return selectedItemIndex; }

    MenuScreen(float width, float height){
		// Initialize the values of the variables

		isPlaying = true;

		isActive[0] = false;
		isActive[1] = false;

		triangleY = 0.325;
		currentState = MAIN_MENU;	// Initialize the current state to the main menu
		loadResources();

		sound_ambient.setBuffer(buffer);
		sound_ambient.setVolume(50.f);
		sound_ambient.setLoop(true);
		sound_ambient.play();

		rectangulo.setPosition(0, 0);
		rectangulo.setSize(sf::Vector2f(width / 2, height));

		textureSize = backgroundTexture.getSize();
		scaleX = static_cast<float>(width) / textureSize.x;
		scaleY = static_cast<float>(height) / textureSize.y;

		// Configuración del triángulo selector
		selectorTriangle.setPointCount(3);
		selectorTriangle.setPoint(0, sf::Vector2f(0, 0));
		selectorTriangle.setPoint(1, sf::Vector2f(20, 10)); // Size of the triangle
		selectorTriangle.setPoint(2, sf::Vector2f(0, 20));
		selectorTriangle.setFillColor(sf::Color::White); // Size of the triangle

		// Place the triangle at the first menu item
		selectorTriangle.setPosition(width / 2 * 0.12, rectangulo.getSize().y * triangleY);

		// Set the scale of the background
		background.setScale(scaleX, scaleY);

		// Create a texture from the background image
		background.setTexture(backgroundTexture);

		Title.setFont(titleFont);
		Title.setString("Forgotten Meadows");
		Title.setCharacterSize(55);
		Title.setFillColor(sf::Color::White);

		sf::FloatRect textRect = Title.getLocalBounds();
		Title.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

		Title.setPosition(width / 2 * 0.5, rectangulo.getSize().y * 0.15);

		menu[0].setFont(font);
		menu[0].setFillColor(sf::Color::White);
		menu[0].setString("Play");
		menu[0].setCharacterSize(45);
		menu[0].setPosition(width / 2 * 0.17, rectangulo.getSize().y * 0.30);

		menu[1].setFont(font);
		menu[1].setFillColor(sf::Color(88, 98, 115));
		menu[1].setString("Options");
		menu[1].setCharacterSize(45);
		menu[1].setPosition(width / 2 * 0.17, rectangulo.getSize().y * 0.47);

		menu[2].setFont(font);
		menu[2].setFillColor(sf::Color(88, 98, 115));
		menu[2].setString("Exit");
		menu[2].setCharacterSize(45);
		menu[2].setPosition(width / 2 * 0.17, rectangulo.getSize().y * 0.64);

		selectedItemIndex = 0;

		// Configure elements for the options
		optionsMenu[0].setFont(font);
		optionsMenu[0].setFillColor(sf::Color::White);
		optionsMenu[0].setString("Freecam");
		optionsMenu[0].setCharacterSize(45);
		optionsMenu[0].setPosition(width / 2 * 0.17, rectangulo.getSize().y * 0.30); // Ajusta la posición según sea necesario


		toggleShape[0].setSize(sf::Vector2f(50, 25));
		toggleShape[0].setFillColor(sf::Color(88, 98, 115));
		toggleShape[0].setPosition(width / 2 * 0.6, rectangulo.getSize().y * 0.326);


		circleShape[0].setRadius(10);
		circleShape[0].setFillColor(sf::Color::Black);
		circleShape[0].setPosition(toggleShape[0].getPosition().x + 2.5f, toggleShape[0].getPosition().y + 2.5f);


		optionsMenu[1].setFont(font);
		optionsMenu[1].setFillColor(sf::Color(88, 98, 115));
		optionsMenu[1].setString("Sunset");
		optionsMenu[1].setCharacterSize(45);
		optionsMenu[1].setPosition(width / 2 * 0.17, rectangulo.getSize().y * 0.47);


		toggleShape[1].setSize(sf::Vector2f(50, 25));
		toggleShape[1].setFillColor(sf::Color(88, 98, 115));
		toggleShape[1].setPosition(width / 2 * 0.6, rectangulo.getSize().y * 0.496);


		circleShape[1].setRadius(10);
		circleShape[1].setFillColor(sf::Color::Black);
		circleShape[1].setPosition(toggleShape[1].getPosition().x + 2.5f, toggleShape[1].getPosition().y + 2.5f);


		// Options to go back to the main menu
		optionsMenu[2].setFont(font);
		optionsMenu[2].setFillColor(sf::Color(88, 98, 115));
		optionsMenu[2].setString("Back");
		optionsMenu[2].setCharacterSize(45);
		optionsMenu[2].setPosition(width / 2 * 0.17, rectangulo.getSize().y * 0.64);


    }

	void draw(sf::RenderWindow& window)
	{
		window.draw(background);
		if (currentState == MAIN_MENU) {
			window.draw(Title);
			window.draw(selectorTriangle);
			Title.setString("Forgotten Meadows");

			for (int i = 0; i < MAX_NUMBER_OF_ITEMS; i++) {
				window.draw(menu[i]);
			}
		}
		else if (currentState == OPTIONS_MENU) {
			// Chage the title of the menu to "Options"
			Title.setString("Options");
			window.draw(Title);
			window.draw(selectorTriangle);
			for (int i = 0; i < MAX_NUMBER_OF_OPTIONS_ITEMS; i++) {
				window.draw(optionsMenu[i]);
				if (i > 0) {
					window.draw(toggleShape[i - 1]);
					window.draw(circleShape[i - 1]);
				}
			}
		}
	}

	void MoveUp()
	{
		if (currentState == MAIN_MENU) {
			if (selectedItemIndex - 1 >= 0) {
				menu[selectedItemIndex].setFillColor(sf::Color(88, 98, 115));
				selectedItemIndex--;
				triangleY -= 0.170;
				menu[selectedItemIndex].setFillColor(sf::Color::White);

				// Move the selector triangle to the new position
				selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);
			}
		}
		else if (currentState == OPTIONS_MENU) {
			if (selectedItemIndex - 1 >= 0) {
				optionsMenu[selectedItemIndex].setFillColor(sf::Color(88, 98, 115));
				selectedItemIndex--;
				triangleY -= 0.170;
				optionsMenu[selectedItemIndex].setFillColor(sf::Color::White);

				// Move the selector triangle to the new position
				selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);
			}
		}
	}

	void MoveDown()
	{
		if (!currentState) {
			if (selectedItemIndex + 1 < MAX_NUMBER_OF_ITEMS) {
				menu[selectedItemIndex].setFillColor(sf::Color(88, 98, 115));
				selectedItemIndex++;
				triangleY += 0.170;
				menu[selectedItemIndex].setFillColor(sf::Color::White);

				// Move the selector triangle to the new position
				selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);

			}
		}
		else if (currentState) {
			if (selectedItemIndex + 1 < MAX_NUMBER_OF_OPTIONS_ITEMS) {
				optionsMenu[selectedItemIndex].setFillColor(sf::Color(88, 98, 115));
				selectedItemIndex++;
				triangleY += 0.170;
				optionsMenu[selectedItemIndex].setFillColor(sf::Color::White);

				selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);
			}
		}
	}

	bool loadResources() {

		bool success = true;
		success &= backgroundTexture.loadFromFile(FileSystem::getPath("resources/menu/images/Menu.png"));
		success &= titleFont.loadFromFile(FileSystem::getPath("resources/menu/textFonts/Poppins-Regular.ttf"));
		success &= font.loadFromFile(FileSystem::getPath("resources/menu/textFonts/Poppins-Regular.ttf"));
		success &= buffer.loadFromFile(FileSystem::getPath("resources/audio/nostalgic-piano.mp3"));

		if (!success) {
			std::cerr << "Failed to load one or more resources" << std::endl;
		}

		return success;
	}

	void changeState(MenuState newState) {
		currentState = newState;
		selectedItemIndex = 0; // Reset the selected item index
		triangleY = 0.325; // Reset the position of the selector triangle

		if (currentState == MAIN_MENU) {
			// Resete the colors of the main menu items
			for (int i = 0; i < MAX_NUMBER_OF_OPTIONS_ITEMS; i++) {
				optionsMenu[i].setFillColor(sf::Color(88, 98, 115)); // Color not selected
			}
			// Set the color of the first item of the main menu as selected
			menu[0].setFillColor(sf::Color::White);

			selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);
		}
		else if (currentState == OPTIONS_MENU) {


			// Reset the colors of the options menu items
			for (int i = 0; i < MAX_NUMBER_OF_ITEMS; i++) {
				menu[i].setFillColor(sf::Color(88, 98, 115)); // Color not selected
			}
			// Set the color of the first item of the options menu as selected
			optionsMenu[0].setFillColor(sf::Color::White);
			selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);
		}

	}

	void toggle(int a) {
		isActive[a] = !isActive[a];
		if (isActive[a]) {
			toggleShape[a].setFillColor(sf::Color(19, 93, 118));
			circleShape[a].setFillColor(sf::Color::White);
			circleShape[a].setPosition(toggleShape[a].getPosition().x + 27.5f, toggleShape[a].getPosition().y + 2.5f);
		}
		else {
			toggleShape[a].setFillColor(sf::Color(88, 98, 115));
			circleShape[a].setFillColor(sf::Color::Black);
			circleShape[a].setPosition(toggleShape[a].getPosition().x + 2.5f, toggleShape[a].getPosition().y + 2.5f);
		}

	}

    int run(sf::RenderWindow& window, Context &context) override {

		if (!isPlaying) {
			sound_ambient.setLoop(true);
			sound_ambient.play();
			isPlaying = true;
		}
		sf::Event event;

		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					MoveUp();
					break;

				case sf::Keyboard::Down:
					MoveDown();
					break;

				case sf::Keyboard::Return:
					if (GetPressedItem() == 2 && getCurrentState() == OPTIONS_MENU) {
						changeState(MAIN_MENU);
					}
					else {
						if (getCurrentState() == MAIN_MENU) {
							switch (GetPressedItem()) {
							case 0:
								//std::cout << "Play button has been pressed" << std::endl;
								sound_ambient.setLoop(false);
								sound_ambient.stop();
								isPlaying = false;

								if (context.SaveStates) {
									window.popGLStates();
								}

								context.Freecam = isActive[0];
								context.Sunset = isActive[1];

								return 1;
								break;
							case 1:
								//std::cout << "Option button has been pressed" << std::endl;
								changeState(OPTIONS_MENU);
								break;
							case 2:
								window.close();
								return -1;
								break;
							}
						}
						else if (getCurrentState() == OPTIONS_MENU) {
							switch (GetPressedItem()) {
							case 0:
								toggle(0);
								//std::cout << "Freecam button has been pressed" << std::endl;
								break;
							case 1:
								toggle(1);
								//std::cout << "Sunset button has been pressed" << std::endl;
								break;
							case 2:
								//std::cout << "Back button has been pressed" << std::endl;
								changeState(MAIN_MENU);
								break;
							}
						}
					}
					break;
				}

				break;
			case sf::Event::Closed:
				window.close();

				break;

			}
		}

		window.clear();

		draw(window);

		window.display();

        return 0;
    }
private:

    
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

	// Audio ambient
	sf::SoundBuffer buffer;
	sf::Sound sound_ambient;
	bool isPlaying;
};