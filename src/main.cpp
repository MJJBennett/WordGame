#include <SFML/Graphics.hpp>
#include "render.hpp"
#include "table.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "WordGame");
    wg::Table table;
    wg::Renderer renderer(window);

    while (window.isOpen())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
        }

        window.clear();

        renderer.render(table);

        window.display();
    }

    return 0;
}
