#ifndef WG_WINDOW_CONTEXT_HPP
#define WG_WINDOW_CONTEXT_HPP

#include <SFML/Graphics.hpp>

namespace wg
{
class WindowContext
{
public:
    template<typename... Args>
    WindowContext(Args... args) : window_(std::forward<Args>(args)...) {}

    ~WindowContext();

    sf::RenderWindow& getTarget() { return window_; }
    bool isOpen() { return window_.isOpen(); }
    bool shouldClose(const sf::Event& e) { return e.type == sf::Event::Closed; }
    void close() { window_.close(); }

    unsigned int width() { return window_.getSize().x; } 

private:
    sf::RenderWindow window_;
};
}  // namespace wg

#endif  // WG_WINDOW_CONTEXT_HPP
