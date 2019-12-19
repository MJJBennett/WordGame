#ifndef WG_RESOURCE_HPP
#define WG_RESOURCE_HPP

// For now we'll put all the includes
// just straight into this file.
// It's always possible to make things into
// pointers later if it really becomes necessary.
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#ifdef _WIN32
#include <cassert>
#endif

namespace wg
{
struct Resource
{
    virtual sf::Drawable& asDrawable() { assert(false); abort();  }
    virtual void setPosition(float, float) { assert(false); }
};

struct FontResource : Resource
{
    sf::Font font;

    FontResource(const std::string& file)
    {
        if (!font.loadFromFile(file)) return;
    }
};

struct TextResource : Resource
{
    sf::Text text;

    TextResource(const sf::Font& font, const std::string& str,
                 const sf::Color& colour = sf::Color::Black, int size = 28)
    {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(colour);
    }

    void setPosition(float x, float y) override { text.setPosition(x, y); }

    sf::Drawable& asDrawable() override { return text; }
};
}  // namespace wg

#endif  // WG_RESOURCE_HPP
