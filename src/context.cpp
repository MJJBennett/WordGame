#include "game/context.hpp"

#include <SFML/Window/Event.hpp>
#include <nlohmann/json.hpp>
#include "assert.hpp"
#include "commands.hpp"
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include "framework/json_tools.hpp"
#include "framework/render.hpp"
#include "framework/tools.hpp"
#include "framework/window_io.hpp"
#include "game/game_io.hpp"
#include "update_handler.hpp"

using json = nlohmann::json;

wg::GameContext::GameContext(wg::WindowContext& c, wg::ResourceManager& r, wg::UpdateHandler& u)
    : io_(c, r, u, board_), update_handler(u)
{
}

void wg::GameContext::init()
{
    load_config("config.json");
    io_.init();
    update_handler.update(wg::ServUpdate{"join", io_.user_});
    update_handler.update(wg::ConfUpdate{"join", io_.user_});
    rect_.setSize(sf::Vector2f{25.f, 25.f});
    std::string legend_str;
    for (int i = 1; i < 5; i++)
    {
        legend_str += wg::multiplier::to_string(wg::multiplier::decode(i)) + '\n';
    }

    io_.setup_text(playerlist_, playerlist_str_);
    io_.setup_text(legend_, legend_str);
    io_.position_playerlist(playerlist_);
    io_.position_playerlist(legend_);
    legend_.move(25, -200);
    legend_.setLineSpacing(1.2);
    fix_playerlist();
}

void wg::GameContext::update()
{
    // Poll once per tick for now
    // This is input, not output
    const auto ou = update_handler.poll_game(true);
    if (ou)
    {
        const auto u = *ou;
        set_tile(u.col, u.row, u.c);
        return;
    }
    const auto ocu = update_handler.poll_chat(true);
    if (ocu)
    {
        const auto cu = *ocu;
        // wg::log::data("Received chat update", cu.message, cu.sender);
        io_.chat(cu.message, cu.sender);
        return;
    }
    const auto oconf = update_handler.poll_conf(true);
    if (oconf)
    {
        const auto conf = *oconf;
        wg::log::point("Updating configuration in some way.");
        if (conf.config == "command")
        {
            if (conf.setting == wg::command::host)
            {
                // We are now the host
                io_.chat_broadcast(io_.user_ + " is now host!", "Server");
                // This is a lie, this isn't the server...
                // Oh well.

                // We're the host now.
                is_host_ = true;
                return;
            }
            return;
        }
        if (conf.config == "join")
        {
            players_.insert({conf.setting, 0});
            fix_playerlist();
            if (is_host_) send_playerlist();
            io_.chat(conf.setting + " has joined the game!", "Server");
            return;
        }
        if (conf.config == "disconnect")
        {
            players_.erase(conf.setting);
            fix_playerlist();
            io_.chat(conf.setting + " has disconnected from the game!", "Server");
            return;
        }
        if (conf.config == "playerlist")
        {
            const auto np = wg::decode_range(conf.setting);
            for (auto&& p : np)
                if (p != io_.user_) players_.insert({p, 0});
            wg::log::point("Updated playerlist.");
            fix_playerlist();
            return;
        }
        if (conf.config == "turn")
        {
            do_turn(conf.setting);
            return;
        }
        if (conf.config == "layout")
        {
            io_.chat("Updating layout.", "Server");
            if (const auto l = wg::try_parse(conf.setting); l)
            {
                board_.set_layout(*l);
            }
            return;
        }
        if (conf.config == "charset")
        {
            io_.charset_ = conf.setting;
            return;
        }
        if (conf.config == "drawtill")
        {
            if (const int i = wg::atoi_default(conf.setting); i > 0)
            {
                io_.draw_tiles(i - io_.hand_.size());
            }
            return;
        }
        if (conf.config == "charscores")
        {
            board_.scores_.from_string(conf.setting);
            return;
        }
        wg::log::warn(
            "Found configuration update that could not be understood by the game context:",
            "\n\tName: ", conf.config, "\n\tValue: ", conf.setting);
        return;
    }
    const auto ojsonu = update_handler.poll_json(true);
    if (ojsonu)
    {
        const auto jsonu = *ojsonu;
        if (jsonu.json_.find("layout") != jsonu.json_.end())
        {
            wg::log::point("Rewriting the board.");
            board_.parse_board_update(jsonu.json_["layout"]);
        }
        else
        {
            wg::log::warn("Found JSON update that could not be understood by the game context:\n",
                          jsonu.json_.dump(2));
        }
    }
}

void wg::GameContext::send_playerlist()
{
    std::vector<std::string> temp_playerlist;
    for (auto&& [k, v] : players_) temp_playerlist.push_back(k);
    temp_playerlist.push_back(io_.user_);
    update_handler.update(wg::ConfUpdate{"playerlist", wg::encode_range(temp_playerlist)});
}

void wg::GameContext::parse_input(sf::Event& e)
{
    const bool ret = io_.mode_ == GameIO::Mode::Normal;
    switch (io_.do_event(e))
    {
        case GameIO::Result::None: break;
        case GameIO::Result::Command:
        {
            if (io_.queue_.size() == 0) return;
            const auto& act = io_.queue_.back();
            maybe_command(act);
        }
        default: return;
    }

    if (io_.mode_ == GameIO::Mode::ChatEdit) return;

    switch (e.type)
    {
        case sf::Event::KeyReleased:
        {
            if (e.key.code == sf::Keyboard::Escape)
            {
                parse_escape();
            }
            else if (mode_ == Mode::Default)
            {
                // Okay so KeyReleased actually occurs after TextEntered
                // Which means this still fires in "Mode::Default" after entering
                // a character into the board.
                // Fast fix:
                if (!just_entered_char_) parse_key_released(e);
            }
            just_entered_char_ = false;
            break;
        }
        case sf::Event::TextEntered:
            if (mode_ != Mode::Default) parse_text_entered(e);
        case sf::Event::MouseButtonReleased: parse_mouse_released(e); break;
        default: break;
    }
}

void wg::GameContext::maybe_command(const wg::Action& act)
{
    switch (act.type_)
    {
        case wg::Action::Type::TurnStart:
        {
            // We need to start someone's turn!
            const auto& p = act.input_;
            const auto& u = io_.user_;
            if (players_.find(p) == players_.end() && p != u)
            {
                wg::log::warn("Tried to start turn of non-existing player: '", p, "'");
                return;
            }
            update_handler.update(wg::ConfUpdate{"turn", p});
            do_turn(p);
            break;
        }
        case wg::Action::Type::PushInfo:
        {
            // Push all information about the current state to other users
            // Yes, this might cause race conditions (to be honest, they definitely already
            // existed) I don't think that's a big concern for this for a variety of reasons
            send_playerlist();
        }
        default: break;
    }
}

void wg::GameContext::do_turn(const std::string& player)
{
    if (turn_)
    {
        end_turn();
    }
    if (io_.user_ == player)
    {
        io_.chat("It's your turn!", "Game");
    }
    else
    {
        io_.chat("It's " + player + "'s turn!", "Game");
    }
    turn_ = wg::Turn{player};
}

void wg::GameContext::end_turn()
{
    wg::assert_true(!!turn_);  // There's probably something better
                               // But I always wanted to do this...

    // TODO - In the future, this is where we would serialize the current
    // turn_ object into a file so that the game can be reloaded easily.
    // For now, we simply count the points, then commit the object to the board.

    // Tally points

    // Currently, we assume tiles are placed in a straight line
    // In the future, we could implement diagonals or other rulesets
    const auto& v = turn_->letters_;
    if (v.size() == 0) return;
    int p = board_.score(v);

    // Count points!

    io_.chat(turn_->player_ + " scored " + std::to_string(p) + " points!", "Game");
    const auto pl = turn_->player_;
    if (pl == io_.user_)
        my_score_ += p;
    else
        players_.at(turn_->player_) += p;
    fix_playerlist();
    // Commit the letters to the board, for drawing purposes
    // For now, this does nothing, as tiles don't really exist
    
    io_.end_turn(); // Executes turn end scripts
}

void wg::GameContext::render(wg::Renderer& renderer)
{
    renderer.render(playerlist_);
    if (hide_things_) return;
    renderer.render(io_);
    renderer.render(board_.table_);
    renderer.render(legend_);
    io_.position_playerlist(rect_);  // I lied earlier, THIS is the worst thing
    rect_.move(0, -200);
    for (size_t i = 1; i < 5; i++)
    {
        rect_.setFillColor(board_.multiplier_colour(multiplier::decode(i)));
        renderer.render(rect_);
        rect_.move(0, 25);
    }
}

// Event handling
void wg::GameContext::parse_key_released(sf::Event& e)
{
    wg::assert_true(e.type == sf::Event::KeyReleased);
    switch (e.key.code)
    {
        case sf::Keyboard::Key::C: load_config("config.json"); return;
        case sf::Keyboard::Key::D: print_debug(); return;
        case sf::Keyboard::Key::H: hide_things_ = !hide_things_; return;
        default: return;
    }
}

void wg::GameContext::parse_text_entered(sf::Event& e)
{
    if (mode_ == Mode::SetTile)
    {
        const auto c = get_game_char<std::optional<char>>(e.text.unicode);
        if (c)
        {
            const auto [col, row] = *pending_tile_;
            set_tile(col, row, *c);
            io_.play_tile(*c);
            update_handler.update(GameUpdate{int(col), int(row), *c});
        }
        mode_              = Mode::Default;
        just_entered_char_ = true;
    }
}

void wg::GameContext::set_tile(int col, int row, char c)
{
    // This method is always called when a tile is placed on the board
    // Therefore, this is our entry point for tracking turns
    if (turn_)
    {
        Tile t(c);
        t.board_pos_   = {col, row};
        const auto m   = board_.table_.at(col, row).multipliers_;
        t.multipliers_ = (m ? *m : ::wg::multiplier::None);
        turn_->letters_.emplace_back(std::move(t));
    }
    auto& item      = board_.table_.at(col, row);
    item.character_ = c;
}

void wg::GameContext::parse_mouse_released(sf::Event& e)
{
    const auto x              = e.mouseButton.x;
    const auto y              = e.mouseButton.y;
    const auto [tw, th]       = board_.table_.get_dimensions();
    const auto [tx, ty]       = board_.table_.position_;
    const auto [tilew, tileh] = board_.table_.get_tile_dimensions();
    const auto [offx, offy]   = board_.table_.get_tile_offsets();

    if (x > tx && x < tx + tw && y > ty && y < ty + th)
    {
        // Okay, we need to figure out which row we're clicking in:
        const unsigned int row = static_cast<int>((x - tx) / (tilew + offx));
        const unsigned int col = static_cast<int>((y - ty) / (tileh + offy));
        // TODO - This should all be cached 100%, I think
        // But only if this is a noticeable performance hit, unlikely

        wg::assert_true(row < board_.table_.table_size && row >= 0 &&
                        col < board_.table_.table_size && col >= 0);

        pending_tile_ = {row, col};
        mode_         = Mode::SetTile;
        // wg::log::point("Set mode to SetTile.");
    }
}
void wg::GameContext::parse_escape()
{
    wg::log::point("Parsing escape command.");

    // switch(mode_
    running_ = false;
}

bool wg::GameContext::load_config(std::string filename)
{
    wg::log::point(__func__, ": Loading configuration data.");
    std::ifstream input_file(filename);
    if (!input_file.good())
    {
        wg::log::warn(__func__, ": Could not open file: ", filename);
        return false;
    }
    json config;
    try
    {
        config = json::parse(input_file);
    }
    catch (const json::parse_error& e)
    {
        wg::log::warn(__func__, ": Could not parse json in file: ", filename);
        wg::log::warn("With error message: ", e.what());
        return false;
    }

    input_file.close();

    if (config.find("board") != config.end()) board_.parse_config(config.at("board"));

    if (config.find("game") != config.end()) parse_config(config.at("game"));

    return true;
}

bool wg::GameContext::parse_config(const nlohmann::json& config)
{
    wg::log::data("Using configuration", config.dump());

    // So we can set things like tile colour, etc
    for (const auto& k : config.items())
    {
        switch (k.value().type())
        {
            case json::value_t::array:
            {
                json arr = k.value().get<json>();
                if (arr.size() == 0) continue;
                if (arr[0].type() == json::value_t::number_unsigned)
                {
                    wg::log::point(">>> Loading unsigned array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<unsigned int>>());
                }
                else
                {
                    wg::log::point(">>> Loading string array for key: ", k.key());
                    set_config(k.key(), k.value().get<std::vector<std::string>>());
                }
                break;
            }
            case json::value_t::string:
            {
                wg::log::point(">>> Loading string for key: ", k.key());
                set_config(k.key(), k.value().get<std::string>());
                break;
            }
            default:
                wg::log::warn(__func__, ": Tried to load unsupported JSON: ", k.value().dump(1));
                break;
        }
    }

    return true;
}

bool wg::GameContext::set_config(std::string name, std::vector<std::string> value) { return true; }

bool wg::GameContext::set_config(std::string name, std::vector<unsigned int> value)
{
    if (name == "background-colour")
    {
        background_ = sf::Color{(sf::Uint8)value[0], (sf::Uint8)value[1], (sf::Uint8)value[2]};
    }
    if (name == "board-colour")
    {
        board_.table_.bg_r = value[0];
        board_.table_.bg_g = value[1];
        board_.table_.bg_b = value[2];
    }
    return true;
}

bool wg::GameContext::set_config(std::string name, std::string value) { return true; }

void wg::GameContext::print_debug()
{
    using std::to_string;
    std::string debug_str = "Debug information:";
    auto item             = [&debug_str](const std::string& name, const std::string& value) {
        debug_str += "\n\t - " + name + ": " + value;
    };
    auto item_b = [&debug_str](const std::string& name, bool value) {
        debug_str += "\n\t - " + name + ": " + (value ? "True" : "False");
    };
    // This is awesome! auto lambdas are so great!
    auto item_v = [&debug_str](const std::string& name, const auto& range) {
        debug_str += "\n\t - " + name + ": ";
        for (auto&& val : range)
        {
            debug_str += "\n\t\t - " + to_string(val);
        }
    };
    auto item_m = [&debug_str](const std::string& name, const auto& range, const int ml = 6) {
        debug_str += "\n\t - " + name + ": ";
        int cl = 0;
        for (auto [k, v] : range)
        {
            if (cl++ % ml == 0)
            {
                debug_str += "\n\t\t| ";
            }
            debug_str += std::string{k} + " : " + to_string(v) + " | ";
        }
    };

    // Players
    item("Username", io_.user_);
    item_b("Is Host", is_host_);
    item_b("Is Running", is_host_);
    item_m("Other Users", players_, 1);
    item("Charset", io_.charset_);
    item_m("Charscores", board_.scores_.get_ref());
    item("Hand", io_.hand_);
    if (turn_) item_v("Turn (" + turn_->player_ + ")", turn_->letters_);

    wg::log::point(debug_str);
}
