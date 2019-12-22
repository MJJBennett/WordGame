#include "framework/window_io.hpp"

#include <SFML/Graphics.hpp>
#include <thread>
#include <vector>
#include "debug/log.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/window_item.hpp"
#include "framework/windowcontext.hpp"

static std::optional<std::string> try_get_text(wg::WindowContext& target, sf::Text& text,
                                               std::vector<wg::Button> buttons)
{
    // Okay, we need to create some buttons
    while (target.isOpen())
    {
        sf::Event e;

        while (target.getTarget().pollEvent(e))
        {
            if (target.shouldClose(e))
            {
                target.close();
                continue;
            }
            switch (e.type)
            {
                case sf::Event::MouseButtonPressed:
                {
                    for (auto&& b : buttons)
                    {
                        b.accept_click(e);
                    }
                    break;
                }
                case sf::Event::MouseButtonReleased:
                {
                    for (auto&& b : buttons)
                    {
                        if (b.accept_release(e)) return b.text();
                    }
                    break;
                }
                default: break;
            }
        }

        target.getTarget().clear();
        for (auto&& b : buttons) target.getTarget().draw(b);
        target.getTarget().draw(text);
        target.getTarget().display();

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }
    return {};
}

static std::optional<char> get_ascii(sf::Event e)
{
    assert(e.type == sf::Event::TextEntered);
    if (e.text.unicode < 128) return static_cast<char>(e.text.unicode);
    return {};
}

std::string wg::window_io::get_string(wg::WindowContext& target, wg::ResourceManager& manager,
                                      const std::string& query,
                                      const std::vector<std::string>& options)
{
    // TODO
    // Implement scrolling
    //  - Maybe make a smart rect/smart positionable that can move up/down
    // Implement generalized IO loop
    sf::Text text;
    text.setFont(manager.defaultFont()->font);
    text.setString(query);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::Cyan);
    text.setPosition(center(float(target.width()), text.getGlobalBounds().width), 16);

    std::vector<wg::Button> buttons;
    unsigned int pos_y{25 + 64};
    for (auto&& opt : options)
    {
        buttons.emplace_back(opt, manager.defaultFont()->font, 25, pos_y, target.width() * 0.8,
                             80);
        buttons.back().set_x(center(float(target.width()), buttons.back().w_));
        pos_y += 125;
    }

    auto str = try_get_text(target, text, buttons);

    return (str ? *str : "");
}

void wg::window_io::back_screen(wg::WindowContext& target, wg::ResourceManager& manager,
                                const std::string& message, const std::string& button_message)
{
    sf::Text text;
    text.setFont(manager.defaultFont()->font);
    text.setString(message);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::Cyan);

    std::vector<wg::Button> buttons;
    buttons.emplace_back(button_message, manager.defaultFont()->font, 25, 25, 100, 100);
    try_get_text(target, text, buttons);
}

std::string wg::window_io::get_from_file(wg::WindowContext& target, wg::ResourceManager& manager,
                                         const std::string& message, const std::string& filename)
{
    sf::Text text;
    text.setFont(manager.defaultFont()->font);
    text.setString(message);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::Cyan);
    text.setPosition(center(float(target.width()), text.getGlobalBounds().width), 16);
}
