#include <SFML/Graphics.hpp>
#include "framework/render.hpp"
#include "framework/table.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/resource.hpp"
#include "game/item.hpp"
#include "game/context.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "WordGame");
    wg::Table<wg::Item> table;
    wg::ResourceManager manager;
    wg::Renderer renderer(window, manager);
    manager.load({wg::ResourceType::text, "Hello world!"});
    auto& text = manager.get({wg::ResourceType::text, "Hello world!"})->asDrawable();
    wg::GameContext game;

    while (window.isOpen() && game.running())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
            game.parse_input(e);
        }
        game.update();

        window.clear();

        game.render(renderer);
        window.draw(text);

        window.display();
    }

    return 0;
}
