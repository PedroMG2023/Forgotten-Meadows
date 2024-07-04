// Screen.h
#pragma once

#include <SFML/Graphics.hpp>
#include "Context.h";

class Screen {
public:
	virtual int run(sf::RenderWindow& window, Context& context) = 0;
};
