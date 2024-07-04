#include "Menu.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem.h>

Menu::Menu(float width, float height)
{
	isActive[0] = false;
	isActive[1] = true;

	triangleY = 0.325;
	currentState = MAIN_MENU; // Initialize menu state to main menu
	loadResources();

	rectangulo.setPosition(0, 0);
	rectangulo.setSize(sf::Vector2f(width / 2, height));

	textureSize = backgroundTexture.getSize();
	scaleX = static_cast<float>(width) / textureSize.x;
	scaleY = static_cast<float>(height) / textureSize.y;

	// Configuration of the selector triangle
	selectorTriangle.setPointCount(3);
	selectorTriangle.setPoint(0, sf::Vector2f(0, 0));
	selectorTriangle.setPoint(1, sf::Vector2f(20, 10)); // Size of the triangle
	selectorTriangle.setPoint(2, sf::Vector2f(0, 20));
	selectorTriangle.setFillColor(sf::Color::White); // Color of the triangle

	// Place the triangle at the first menu item
	selectorTriangle.setPosition(width / 2 * 0.12, rectangulo.getSize().y * triangleY);

	// Set scale of the background
	background.setScale(scaleX, scaleY);

	// Create a sprite from the texture
	background.setTexture(backgroundTexture);

	Title.setFont(titleFont);
	Title.setString("Forgotten Meadows");
	Title.setCharacterSize(55);
	Title.setFillColor(sf::Color::White);

	sf::FloatRect textRect = Title.getLocalBounds();
	Title.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	Title.setPosition(width / 2 * 0.5, rectangulo.getSize().y * 0.15);

	// Configure elements of the main menu
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
}


Menu::~Menu()
{
}

void Menu::draw(sf::RenderWindow& window)
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

}

void Menu::MoveUp()
{
	if (currentState == MAIN_MENU) {
		if (selectedItemIndex - 1 >= 0) {
			menu[selectedItemIndex].setFillColor(sf::Color(88, 98, 115));
			selectedItemIndex--;
			triangleY -= 0.170;
			menu[selectedItemIndex].setFillColor(sf::Color::White);

			// Mueve el triángulo selector a la nueva posición
			selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);
		}
	}
}

void Menu::MoveDown()
{
	if (currentState == MAIN_MENU) {
		if (selectedItemIndex + 1 < MAX_NUMBER_OF_ITEMS) {
			menu[selectedItemIndex].setFillColor(sf::Color(88, 98, 115));
			selectedItemIndex++;
			triangleY += 0.170;
			menu[selectedItemIndex].setFillColor(sf::Color::White);

			// Mueve el triángulo selector a la nueva posición
			selectorTriangle.setPosition(getWindowWidth() * 0.12, rectangulo.getSize().y * triangleY);

		}
	}
}

bool Menu::loadResources() {
	bool success = true;
	success &= backgroundTexture.loadFromFile(FileSystem::getPath("resources/menu/images/Menu.png"));
	success &= titleFont.loadFromFile("resources/menu/textFonts/Poppins-Regular.ttf");
	success &= font.loadFromFile("resources/menu/textFonts/Poppins-Regular.ttf");
	if (!success) {
		std::cerr << "Failed to load one or more resources" << std::endl;
	}
	return success;
}

void Menu::toggle(int a) {
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