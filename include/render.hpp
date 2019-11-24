#ifndef WG_RENDER_HPP
#define WG_RENDER_HPP

#include <SFML/Graphics/RectangleShape.hpp>

// Forward declaration
namespace sf
{
class RenderWindow;
class Drawable;
}  // namespace sf

namespace wg
{
class Item;
class Table;

class Renderer
{
public:
    enum class Mode
    {
        vertical,
    };

public:
    Renderer(sf::RenderWindow& window);

    void start_render(unsigned int offset);

    void render(const sf::Drawable&);
    void render(const Item&);
    void render(const Table&);

private:
    sf::RectangleShape rect_;
    sf::RenderWindow& window_;

    Mode mode_{Mode::vertical};
    unsigned int position_{0};
    unsigned int offset_{0};
};
}  // namespace wg

#endif  // WG_RENDER_HPP
