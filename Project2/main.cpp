#include <SFML/Graphics.hpp>
#include <iostream>
#include "Globals.h"
#include "Game.h"

int main()
{
	try
	{
		sf::RenderWindow win(sf::VideoMode({ WIN_WIDTH, WIN_HEIGHT }), "Flappy Bird");
		Game game(win);
		game.startGameLoop();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}