#include "framework/window_io.hpp"

#include <SFML/Graphics.hpp>
#include <optional>
#include <thread>
#include <vector>
#include "assert.hpp"
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/window_context.hpp"
#include "framework/window_item.hpp"

static std::optional<std::string> try_get_text(wg::WindowContext& target, sf::Text& text,
                                               std::vector<wg::Button> buttons)
{
    // Okay, we need to create some buttons
    while (target.isOpen())
    {
        sf::Event e;

        while (target.pollEvent(e))
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
    wg::assert_true(e.type == sf::Event::TextEntered);
    if (e.text.unicode < 128 && e.text.unicode > 31) return static_cast<char>(e.text.unicode);
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
        buttons.emplace_back(opt, manager.defaultFont()->font, 25.f, (float)pos_y,
                             (float)((float)target.width() * 0.8), 80.f);
        buttons.back().set_x(center(float(target.width()), buttons.back().w_));
        pos_y += 125;
    }

    auto str = try_get_text(target, text, buttons);

    return (str ? *str : "");
}

std::string wg::window_io::get_string(wg::WindowContext& target, wg::ResourceManager& manager,
                                      const std::string& query)
{
    sf::Text text;
    text.setFont(manager.defaultFont()->font);
    text.setString(query);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::Cyan);
    text.setPosition(center(float(target.width()), text.getGlobalBounds().width), 16);

    sf::Text input_text;
    input_text.setFont(manager.defaultFont()->font);
    input_text.setCharacterSize(48);
    input_text.setFillColor(sf::Color::Cyan);
    input_text.setPosition(center(float(target.width()), input_text.getGlobalBounds().width), 86);
    std::string input_str;

    while (target.isOpen())
    {
        sf::Event e;

        while (target.pollEvent(e))
        {
            if (target.shouldClose(e))
            {
                target.close();
                continue;
            }
            switch (e.type)
            {
                case sf::Event::KeyReleased:
                {
                    if (e.key.code == sf::Keyboard::Enter) return input_str;
                    if (e.key.code == sf::Keyboard::Backspace)
                    {
                        if (input_str.size() == 0) continue;
                        input_str.pop_back();
                        input_text.setString(input_str);
                        input_text.setPosition(
                            center(float(target.width()), input_text.getGlobalBounds().width), 86);
                        continue;
                    }
                    break;
                }
                case sf::Event::TextEntered:
                {
                    if (auto opt = get_ascii(e); opt)
                    {
                        input_str += *opt;
                        input_text.setString(input_str);
                        input_text.setPosition(
                            center(float(target.width()), input_text.getGlobalBounds().width), 86);
                    }
                    break;
                }
                default: break;
            }
        }

        target.getTarget().clear();
        target.getTarget().draw(text);
        target.getTarget().draw(input_text);
        target.getTarget().display();

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }

    wg::log::point("Got brand new input string: ", input_str);
    return input_str;
}

void wg::window_io::back_screen(wg::WindowContext& target, wg::ResourceManager& manager,
                                const std::string& message, const std::string& button_message)
{
    sf::Text text;
    text.setFont(manager.defaultFont()->font);
    text.setString(message);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::Cyan);
    text.setPosition(center(float(target.width()), text.getGlobalBounds().width), 16);

    std::vector<wg::Button> buttons;
    buttons.emplace_back(button_message, manager.defaultFont()->font, 0.f, 128.f, 250.f, 64.f);
    buttons.back().set_x(center(float(target.width()), buttons.back().w_));
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

    while (target.isOpen())
    {
        auto options = wg::get_lines(filename, wg::ReadMode::HasChars);
        options.push_back("Add New");
        auto str = wg::window_io::get_string(target, manager, message, options);
        if (!target.isOpen()) return "";
        if (str != "Add New") return str;
        wg::add_line(filename, wg::window_io::get_string(target, manager, message));
    }
    return "";
}
