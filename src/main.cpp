#include <SFML/Graphics.hpp>
#include "render.hpp"
#include "table.hpp"
#include "resourcemanager.hpp"
#include "resource.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "WordGame");
    wg::Table table;
    wg::Renderer renderer(window);
    wg::ResourceManager manager;
    manager.load({wg::ResourceType::text, "Hello world!"});
    auto& text = manager.get({wg::ResourceType::text, "Hello world!"})->drawable();

    while (window.isOpen())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
        }

        window.clear();

        renderer.render(table);
        window.draw(text);

        window.display();
    }

    return 0;
}
