#include "framework/window_item.hpp"

wg::Button::Button(std::string text, const sf::Font& font, float x, float y, float w, float h)
    : x_(x), y_(y), w_(w), h_(h)
{
    text_.setFont(font);
    text_.setString(text);
    text_.setCharacterSize(28);
    text_.setFillColor(sf::Color::Black);
    rect_.setSize(sf::Vector2f{w,h});
    rect_.setFillColor(sf::Color{128, 128, 128});
    rect_.setPosition(sf::Vector2f{x,y});
    shade_.setSize(sf::Vector2f{w,h});
    shade_.setFillColor(sf::Color{64, 64, 64, 128});
    shade_.setPosition(sf::Vector2f{x,y});
}

bool wg::Button::accept_click(const sf::Event& e)
{
    assert(e.type == sf::Event::MouseButtonPressed);

    if (is_inside(e.mouseButton.x, e.mouseButton.y)) clicked_ = true;
    return clicked_;
}

bool wg::Button::accept_release(const sf::Event& e)
{
    assert(e.type == sf::Event::MouseButtonReleased);

    bool full_click = clicked_ && is_inside(e.mouseButton.x, e.mouseButton.y);
    reset();

    return full_click;
}

bool wg::Button::is_inside(int x, int y)
{
    if ((x > x_) && (x < x_ + w_) && (y > y_) && (y < y_ + h_)) return true;
    return false;
}
