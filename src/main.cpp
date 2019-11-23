#include <SFML/Window.hpp>

int main()
{
	sf::Window window(sf::VideoMode(800, 600), "My window");

	// Run the program as long as the window is open
	while (window.isOpen())
	{
		// Check all the window's events that were triggered
		// since the last iteration of the loop
		// `event` is a reserved keyword in C++, 
		// hence why use `e` instead
		sf::Event e;

		while (window.pollEvent(e))
		{
			if (event.type == sf::Event::Closed)
			window.close();
		}
	}

	return 0;
}
