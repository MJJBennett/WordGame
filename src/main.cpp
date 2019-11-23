#include <SFML/Window.hpp>
#include "table.hpp"

int main()
{
	sf::Window window(sf::VideoMode(800, 600), "WordGame");
    wg::Table table;

	while (window.isOpen())
	{
		sf::Event e;

		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
			window.close();
		}
	}

	return 0;
}
