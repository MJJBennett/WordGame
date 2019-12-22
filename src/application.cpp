#include "application.hpp"

#include <SFML/Graphics.hpp>
#include <thread>
#include "client.hpp"
#include "debug/log.hpp"
#include "framework/render.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/webclient.hpp"
#include "framework/window_io.hpp"
#include "framework/windowcontext.hpp"
#include "game/context.hpp"
#include "game/item.hpp"
#include "server.hpp"

int wg::Application::launch()
{
    wg::WindowContext window(sf::VideoMode(800, 600), "WordGame");
    wg::ResourceManager manager;
    wg::Renderer renderer(window, manager);

    // Example load code, unused, does populate default font though
    // Which isn't really the ideal way to do things, but hey!
    manager.load({wg::ResourceType::text, "Hello world!"});
    auto& text = manager.get({wg::ResourceType::text, "WordGame"})->asDrawable();

    std::vector<std::string> opts;
    opts.push_back("Launch Server");
    opts.push_back("Connect to Server");
    opts.push_back("Play Singleplayer (Lonely Mode)");
    const auto choice = wg::window_io::get_string(window, manager, "Game Menu", opts);
    wg::log::point("Entering mode: ", choice);

    if (choice == "Launch Server") run_webserver(window, manager, renderer);
    else if (choice == "Connect to Server") run_webclient(window, manager, renderer);
    else if (choice == "Play Singleplayer (Lonely Mode)") run_local(window, manager, renderer);

    return 0;
}

int wg::Application::run_webserver(wg::WindowContext& window, wg::ResourceManager& manager,
                                   wg::Renderer& renderer)
{
    const auto addr =
        wg::window_io::get_from_file(window, manager, "Enter Your IP Address", "ip.txt");
    wg::window_io::back_screen(
        window, manager, "The window will now close.\nThe server will run in terminal.", "Okay");
    if (!window.isOpen()) return 0;
    window.close();
    window.getTarget().setVisible(false);
    wg::Server s(addr);
    return 0;
}

int wg::Application::run_webclient(wg::WindowContext& window, wg::ResourceManager& manager,
                                   wg::Renderer& renderer)
{
    const auto addr =
        wg::window_io::get_from_file(window, manager, "Enter Remote IP Address", "ip.txt");
    wg::GameContext game;
    wg::web::Client web_client;
    web_client.launch(addr, "27600");

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

        window.getTarget().display();

        // let's just wait for a little bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    web_client.shutdown(true);
    return 0;
}

int wg::Application::run_local(wg::WindowContext& window, wg::ResourceManager& manager,
                               wg::Renderer& renderer)
{
    wg::window_io::back_screen(window, manager, "This mode is currently unsupported.", "Oh... :(");
    return 0;
}
