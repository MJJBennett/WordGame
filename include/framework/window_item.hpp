#ifndef WG_WINDOW_ITEM_HPP
#define WG_WINDOW_ITEM_HPP

#include <SFML/Graphics.hpp>

namespace wg
{
class Button : public sf::Drawable
{
public:
    float x_;
    float y_;
    float w_;
    float h_;
    Button(std::string text, const sf::Font& font, float x, float y, float w, float h);
    bool accept_click(const sf::Event& e);
    bool accept_release(const sf::Event& e);
    bool reset()
    {
        bool ret = clicked_;
        clicked_ = false;
        return ret;
    }
    bool is_inside(int x, int y);

private:
    bool clicked_{false};
    sf::Text text_;
    sf::RectangleShape rect_;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(text_);
        target.draw(rect_);
    }
};
}  // namespace wg

#endif  // WG_WINDOW_ITEM_HPP
