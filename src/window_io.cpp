#include "framework/window_io.hpp"

#include <SFML/Graphics.hpp>
#include <thread>
#include <vector>
#include "debug/log.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/window_item.hpp"
#include "framework/windowcontext.hpp"

std::string wg::window_io::get_string(wg::WindowContext& target, wg::ResourceManager& manager,
                                      const std::string& query,
                                      const std::vector<std::string>& options)
{
    std::vector<wg::Button> buttons;
    buttons.emplace_back("Test", manager.defaultFont()->font, 25, 25, 100, 100);
    int do_exit = 0;
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
                        if (b.accept_release(e))
                        {
                            wg::log::point("Got a full click on a button!");
                            if (do_exit++ > 5) return "";
                        }
                    }
                    break;
                }
                default: break;
            }
        }

        target.getTarget().clear();
        target.getTarget().draw(buttons[0]);
        target.getTarget().display();

        std::this_thread::sleep_for(std::chrono::milliseconds(80));
    }

    return "";
}
