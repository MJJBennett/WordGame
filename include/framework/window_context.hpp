#ifndef WG_WINDOW_CONTEXT_HPP
#define WG_WINDOW_CONTEXT_HPP

#include <SFML/Graphics.hpp>

namespace wg
{
class WindowContext
{
public:
    template <typename... Args>
    WindowContext(Args... args) : window_(std::forward<Args>(args)...)
    {
    }

    ~WindowContext();

    sf::RenderWindow& getTarget() { return window_; }
    bool isOpen() { return window_.isOpen(); }
    bool shouldClose(const sf::Event& e) { return e.type == sf::Event::Closed; }
    void close() { window_.close(); }

    bool pollEvent(sf::Event& e)
    {
        auto b = window_.pollEvent(e);
        if (b && e.type == sf::Event::Resized)
        {
            auto v = window_.getDefaultView();
            v.setSize(static_cast<float>(e.size.width), static_cast<float>(e.size.height));
            window_.setView(v);
        }
        return b;
    }

    unsigned int width() { return window_.getSize().x; }
    unsigned int height() { return window_.getSize().y; }

private:
    sf::RenderWindow window_;
};
}  // namespace wg

#endif  // WG_WINDOW_CONTEXT_HPP
