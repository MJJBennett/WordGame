#ifndef WG_WINDOW_ITEM_HPP
#define WG_WINDOW_ITEM_HPP

#include <SFML/Graphics.hpp>
#include "graphics/display_mode.hpp"

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
    std::string text() { return text_.getString(); }

    void set_x(float x);
    void update_position();
    void set_mode(wg::DisplayMode m) { mode_ = m; }

private:
    bool clicked_{false};
    sf::Text text_;
    sf::RectangleShape rect_;
    sf::RectangleShape shade_;
    wg::DisplayMode mode_{DisplayMode::center};

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(rect_);
        target.draw(text_);
        if (clicked_) target.draw(shade_);
    }
};
}  // namespace wg

#endif  // WG_WINDOW_ITEM_HPP
