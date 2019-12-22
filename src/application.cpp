#include "application.hpp"

#include <SFML/Graphics.hpp>
#include <thread>
#include "client.hpp"
#include "debug/log.hpp"
#include "framework/render.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/table.hpp"
#include "framework/webclient.hpp"
#include "framework/window_io.hpp"
#include "framework/windowcontext.hpp"
#include "game/context.hpp"
#include "game/item.hpp"
#include "server.hpp"

int wg::Application::launch()
{
    wg::WindowContext window(sf::VideoMode(800, 600), "WordGame");
    wg::Table<wg::Item> table;
    wg::ResourceManager manager;
    wg::Renderer renderer(window, manager);
    manager.load({wg::ResourceType::text, "Hello world!"});
    auto& text = manager.get({wg::ResourceType::text, "WordGame"})->asDrawable();
    wg::GameContext game;
    wg::web::Client web_client;
    //web_client.launch("127.0.0.1", "27600");

    std::vector<std::string> opts;
    opts.push_back("Test 1");
    opts.push_back("Test 2");
    opts.push_back("Test 3");
    wg::log::data("String configured",
                  wg::window_io::get_string(window, manager, "What do you want?", opts));

    while (window.isOpen() && game.running())
    {
        sf::Event e;

        while (window.getTarget().pollEvent(e))
        {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyReleased) web_client.send("KeyEvent occurred!");
            game.parse_input(e);
        }
        game.update();

        window.getTarget().clear();

        game.render(renderer);
        window.getTarget().draw(text);

        window.getTarget().display();

        // let's just wait for a little bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    web_client.shutdown(true);

    return 0;
}
