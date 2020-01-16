#include "application.hpp"

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <thread>
#include "client.hpp"
#include "debug/log.hpp"
#include "dev/devclient.hpp"
#include "framework/render.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/webclient.hpp"
#include "framework/window_context.hpp"
#include "framework/window_io.hpp"
#include "game/context.hpp"
#include "game/item.hpp"
#include "game/multipliers.hpp"
#include "game/settings.hpp"
#include "server.hpp"

int wg::Application::launch(Mode mode)
{
    if (mode & ApplicationMode::Windowless) return run_windowless();
    wg::WindowContext window(sf::VideoMode(800, 600), "WordGame");
    wg::ResourceManager manager;
    wg::Renderer renderer(window, manager);

    // Example load code, unused, does populate default font though
    // Which isn't really the ideal way to do things, but hey!
    manager.load({wg::ResourceType::text, "Hello world!"});
    auto& text = manager.get({wg::ResourceType::text, "WordGame"})->asDrawable();

    while (true)
    {
        std::vector<std::string> opts;
        opts.push_back("Launch Server");
        opts.push_back("Connect to Server");
        opts.push_back("Play Singleplayer (Lonely Mode)");
        if (mode & ApplicationMode::Developer) opts.push_back("Develop");
        const auto choice = wg::window_io::get_string(window, manager, "Game Menu", opts);
        wg::log::point("Entering mode: ", choice);

        if (choice == "Launch Server")
        {
            if (run_webserver(window, manager, renderer) == 0) break;
        }
        else if (choice == "Connect to Server")
        {
            if (run_webclient(window, manager, renderer) == 0) break;
        }
        else if (choice == "Play Singleplayer (Lonely Mode)")
        {
            if (run_local(window, manager, renderer) == 0) break;
        }
        else if (choice == "Develop")
        {
            if (run_develop(window, manager, renderer) == 0) break;
        }
        else
            break;
    }

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
    return run_webserver(addr);
}

int wg::Application::run_windowless()
{
    std::cout << "Starting webserver.";
    if (address_.size() < 4)
    {
        std::cout << " Enter IP:\n";
        std::getline(std::cin, address_);
    }
    else
    {
        std::cout << "\n";
    }
    std::cout << "Running with IP: " << address_ << std::endl;
    return run_webserver(address_);
}

int wg::Application::run_webserver(std::string address)
{
    wg::Server s(address);
    return 0;
}

int wg::Application::run_webclient(wg::WindowContext& window, wg::ResourceManager& manager,
                                   wg::Renderer& renderer)
{
    const auto addr =
        wg::window_io::get_from_file(window, manager, "Enter Remote IP Address", "ip.txt");
    wg::web::Client web_client;
    // TODO - add 'connecting' pane here
    // when connection fails (if), use wg::window_io::back_screen and go back to main menu
    // basically, we want a blocking "wait" on connection
    web_client.launch(addr, "27600");
    wg::Settings settings;
    wg::GameContext game(window, manager, web_client, settings);
    game.init();
    wg::log::point("Initialized game.");

    while (window.isOpen() && game.running())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
                continue;
            }
            game.parse_input(e);
        }
        game.update();

        window.getTarget().clear(game.background_);

        game.render(renderer);

        window.getTarget().display();

        // let's just wait for a little bit
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        // check for messages on the web client...
        web_client.cache_once();
    }

    wg::log::point("Shutting down web client.");
    web_client.shutdown(true);
    return 0;
}

int wg::Application::run_develop(wg::WindowContext& window, wg::ResourceManager& manager,
                                 wg::Renderer& renderer)
{
    wg::dev::Client client;
    wg::Settings settings;
    wg::GameContext game(window, manager, client, settings);
    game.init();
    wg::log::point("Initialized game.");
    std::vector<sf::Text> help;
    for (int i = 0; i < 6; i++)
    {
        // Ah, reflection
        const std::string s =
            std::to_string(i) + " is: " + wg::multiplier::to_string(wg::multiplier::decode(i));
        help.push_back(sf::Text{s, manager.defaultFont()->font, 24});
        help.back().setPosition(window.width() - 270, i * 32);
        help.back().setFillColor(sf::Color{1, 1, 1});
    }

    while (window.isOpen() && game.running())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
                continue;
            }
            game.parse_input(e);
        }
        game.update();

        window.getTarget().clear(game.background_);

        game.render(renderer);
        for (auto&& t : help) window.getTarget().draw(t);

        window.getTarget().display();

        // let's just wait for a little bit
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        // check for messages on the web client...
    }

    return 0;
}

int wg::Application::run_local(wg::WindowContext& window, wg::ResourceManager& manager,
                               wg::Renderer& renderer)
{
    wg::window_io::back_screen(window, manager, "This mode is currently unsupported.", "Oh... :(");
    return 0;
}

void wg::Application::set_address(std::string address)
{
    wg::log::point("Application: Setting address to: '", address, "'");
    address_ = address;
}

void wg::Application::set_port(std::string port)
{
    wg::log::point("Application: Setting port to: '", port, "'");
    wg::log::point("(Note: This does nothing)");
    port_ = port;
}
