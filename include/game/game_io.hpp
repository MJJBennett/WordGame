#ifndef WG_GAME_IO_HPP
#define WG_GAME_IO_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <queue>
#include <string>
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
        TurnStart,
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

    enum class Result
    {
        None,
        Command,
        BoardEdited,
        ChatEdited,
        ModeEdited,
        Ignore,  // Like None, but makes Context ignore the event
    };

public:
    GameIO(wg::WindowContext& target, wg::ResourceManager& manager,
           wg::UpdateHandler& update_handler, wg::Board& board);
    void init();

    Result do_event(const sf::Event&);
    Result text_entered(unsigned int c);
    Result key_pressed(sf::Keyboard::Key k);
    Result key_released(sf::Keyboard::Key k);
    Result do_enter();

    void chat(std::string msg, std::string auth);
    void chat_broadcast(std::string msg, std::string auth);

    std::string user_;
    std::optional<Action> partial_action_;
    std::queue<Action> queue_;
    void log_queue();

    bool handle_command(const std::string& command);
    void load_charset(std::string charset);
    void draw_tiles(int num);

    std::string charset_;
    std::string hand_;
    sf::Text hand_text_;
    bool play_tile(char c)
    {
        if (auto it = hand_.find(c); it != std::string::npos)
        {
            hand_.erase(it);
            hand_text_.setString("Hand: " + hand_);
            return true;
        }
        return false;
    }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    wg::WindowContext& target_;
    wg::ResourceManager& manager_;
    wg::UpdateHandler& update_handler_;
    wg::Board& board_;
    std::vector<sf::Text> chat_bar_;
    sf::Text chat_text_;

    // Appearance
    const int message_bar_height_{24};      // height of a message
    const int message_character_size_{18};  // size of character in message
    const int message_left_offset_{12};     // left offset of the chat
    const int hand_character_size_{18};
};
}  // namespace wg

#endif  // WG_GAME_IO_HPP
