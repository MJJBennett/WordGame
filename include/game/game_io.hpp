#ifndef WG_GAME_IO_HPP
#define WG_GAME_IO_HPP

#include <queue>
#include <string>
#include <optional>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include "wg_forward.hpp"

namespace wg
{
// Each player input that occurs is represented by
// an Action. Multiple Actions can combine into a larger
// Action (for example, large text input). The IO handler
// here will track this and queue updates that need to be
// made to the game in Actions.
struct Action
{
    enum class Type
    {
        BoardCharacter,
        BoardWord,

        ChatCharacter,
        ChatWord,

        CommandBind,
    } type_;

    std::string input_;
};

class GameIO : public sf::Drawable
{
public:
    enum class Mode
    {
        BoardEdit,
        ChatEdit,
        Normal,
    } mode_ = Mode::Normal;

public:
    GameIO(wg::WindowContext& target, wg::ResourceManager& manager, wg::UpdateHandler& update_handler);

    bool do_event(const sf::Event&);
    void text_entered(unsigned int c);
    void key_pressed(sf::Keyboard::Key k);
    void key_released(sf::Keyboard::Key k);
    void do_enter();

    void chat(std::string msg, std::string auth);

    std::optional<Action> partial_action_;
    std::queue<Action> queue_;
    void log_queue();

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    wg::WindowContext& target_;
    wg::ResourceManager& manager_;
    wg::UpdateHandler& update_handler_;
    std::vector<sf::Text> chat_bar_;
    sf::Text chat_text_;
};
}  // namespace wg

#endif  // WG_GAME_IO_HPP
