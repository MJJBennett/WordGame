#ifndef WG_GAME_CONTEXT_HPP
#define WG_GAME_CONTEXT_HPP

#include "game/board.hpp"
#include "game/update.hpp"
#include "game/game_io.hpp"
#include "wg_forward.hpp"

namespace wg
{
class GameContext
{
public:
    GameContext(wg::WindowContext& c, wg::ResourceManager& r, wg::UpdateHandler& u);

    // Everything we have that can be drawn, can receive input
    // We want to be able to manage that - so:
    void parse_input(sf::Event&);

    // We're going to add this for now as empty, but it'll come in handy
    // This runs once per game tick
    void update(){}

    // This is where we render everything!
    void render(wg::Renderer& renderer);

    bool running() { return running_; }

    void set_tile(int col, int row, char c);

    bool load_config(std::string filename);
    bool parse_config(const nlohmann::json& config);
    bool set_config(std::string name, std::vector<std::string> value);
    bool set_config(std::string name, std::vector<unsigned int> value);
    bool set_config(std::string name, std::string value);

public:
    // A game has a table, a chat, a network context, and a few other things
    // Let's deal with the table first of all
    wg::Board board_;

    // We set the background here, the application takes it for us.
    sf::Color background_;

private:
    void parse_key_released(sf::Event&);
    void parse_text_entered(sf::Event&);
    void parse_mouse_released(sf::Event&);
    void parse_escape();
private:
    enum class Mode
    {
        Text,
        Default,
        SetTile,
    } mode_ = Mode::Default;

    std::optional<std::array<unsigned int, 2>> pending_tile_;
    wg::UpdateHandler& update_handler;

    bool running_{true};

    GameIO io_;
};
}  // namespace wg

#endif  // WG_GAME_CONTEXT_HPP
