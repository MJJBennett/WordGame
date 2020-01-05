#include "game/game_io.hpp"

#include <fstream>
#include "debug/log.hpp"
#include "framework/file_io.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/supertools.hpp"
#include "framework/tools.hpp"
#include "framework/window_context.hpp"
#include "framework/window_io.hpp"
#include "game/board.hpp"
#include "update_handler.hpp"

// This is basically just meant to be a better version of the
// 'window io' static functions.

wg::GameIO::GameIO(wg::WindowContext& target, wg::ResourceManager& manager,
                   wg::UpdateHandler& update_handler, wg::Board& board)
    : target_(target), manager_(manager), update_handler_(update_handler), board_(board)
{
    setup_text(chat_text_);
    chat_text_.setCharacterSize(message_character_size_);
    chat_text_.setPosition(message_left_offset_, target.height() - message_bar_height_);
    setup_text(hand_text_);
    hand_text_.setCharacterSize(hand_character_size_);
    hand_text_.setPosition(target.width() - 200, target.height() / 2);
}

void wg::GameIO::init()
{
    user_ = wg::window_io::get_from_file(target_, manager_, "Enter Username", "name.txt");
}

wg::GameIO::Result wg::GameIO::do_event(const sf::Event& e)
{
    switch (e.type)
    {
        case sf::Event::TextEntered: return text_entered(e.text.unicode);
        case sf::Event::KeyPressed: return key_pressed(e.key.code);
        case sf::Event::KeyReleased: return key_released(e.key.code);
        case sf::Event::Resized: return target_resized(e.size.width, e.size.height);
        default: return Result::None;
    }
}

wg::GameIO::Result wg::GameIO::text_entered(unsigned int c)
{
    // Where do we redirect this?
    // To the board, if we're in 'Board Edit' mode...
    if (mode_ == Mode::BoardEdit)
    {
        auto ch = get_char<std::optional<char>>(c);
        if (ch) queue_.push({Action::Type::BoardCharacter, std::string{*ch}});
        return Result::BoardEdited;
    }
    if (mode_ == Mode::ChatEdit)
    {
        // We only make one action from this
        if (auto ch = get_char<std::optional<char>>(c); ch)
        {
            queue_.push({Action::Type::ChatCharacter, std::string{*ch}});
            if (partial_action_)
            {
                (*partial_action_).input_ += *ch;
            }
            else
            {
                partial_action_ = {Action::Type::ChatWord, std::string{*ch}};
            }
            chat_text_.setString((*partial_action_).input_);
        }
        return Result::ChatEdited;
    }
    return Result::None;
}

void wg::GameIO::chat(std::string msg, std::string auth)
{
    // Move up all the other messages
    for (auto&& ct : chat_bar_)
    {
        ct.move(0, -message_bar_height_);
    }
    // Create an object for the new message
    sf::Text chat_message;
    // Configure the new message object
    chat_message.setFont(manager_.defaultFont()->font);
    chat_message.setCharacterSize(message_character_size_);
    chat_message.setFillColor(sf::Color::Black);
    chat_message.setPosition(message_left_offset_,
                             target_.height() - message_bar_height_ - message_bar_height_);
    // Add the message string
    chat_message.setString(auth + ": " + msg);
    // Add the new message to the list of messages
    chat_bar_.push_back(std::move(chat_message));
}

void wg::GameIO::chat_broadcast(std::string msg, std::string auth)
{
    update_handler_.update(wg::ChatUpdate{msg, auth});
    chat(msg, auth);
}

wg::GameIO::Result wg::GameIO::key_pressed(sf::Keyboard::Key k)
{
    switch (k)
    {
        case sf::Keyboard::Key::Enter: return do_enter();
        case sf::Keyboard::Key::Slash: return do_enter('/');
        default: return Result::None;
    }
}

wg::GameIO::Result wg::GameIO::key_released(sf::Keyboard::Key k)
{
    switch (k)
    {
        case sf::Keyboard::Key::BackSpace:
        {
            if (mode_ == Mode::ChatEdit && partial_action_ && partial_action_->input_.size() != 0)
            {
                partial_action_->input_.pop_back();
                chat_text_.setString((*partial_action_).input_);
            }
        }
        case sf::Keyboard::Key::Enter:
        {
            return Result::None;  // this is done on keypress, see do_enter
        }
        default: return Result::None;
    }
    if (mode_ == Mode::ChatEdit) return Result::ChatEdited;
    return Result::None;
}

wg::GameIO::Result wg::GameIO::target_resized(unsigned int width, unsigned int height)
{
    return Result::None;
}

wg::GameIO::Result wg::GameIO::do_enter(char b)
{
    switch (mode_)
    {
        case Mode::Normal:
        {
            mode_ = Mode::ChatEdit;
            if (b != '\b') text_entered('\b');
            return Result::ModeEdited;
        }
        case Mode::BoardEdit:
        {
            mode_ = Mode::Normal;
            return Result::ModeEdited;
        }
        case Mode::ChatEdit:
        {
            auto r = Result::ModeEdited;
            ;
            if (partial_action_ && partial_action_->type_ == Action::Type::ChatWord &&
                (*partial_action_).input_.length() > 0)
            {
                const std::string input = (*partial_action_).input_;
                if (!has_chars(input))
                {
                    // clear input
                }
                else
                {
                    wg::log::point("Sending message: ", input);

                    if (!handle_command(input))
                    {
                        chat_broadcast(input, user_);
                        queue_.push(*partial_action_);  // Record action
                    }
                    else
                        r = Result::Command;
                }
                chat_text_.setString("");
                partial_action_.reset();
            }
            mode_ = Mode::Normal;
            return r;  // not so sure about this but hey
        }
        default: abort();
    }
}

void wg::GameIO::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(chat_text_);
    for (auto&& t : chat_bar_) target.draw(t);
    target.draw(hand_text_);
}

// Logging, serializing, etc
void wg::GameIO::log_queue()
{
    // This is really unfortunate, so don't call this method often
    std::queue<Action> qcopy = queue_;

    std::string msg = "GameIO Action Queue:\n| Type           | Input";
    while (!qcopy.empty())
    {
        auto a = qcopy.back();
        qcopy.pop();
        std::string msg;
        switch (a.type_)
        {
            case Action::Type::BoardCharacter: msg += "| BoardCharacter | "; break;
            case Action::Type::BoardWord: msg += "| BoardWord      | "; break;
            case Action::Type::ChatCharacter: msg += "| ChatCharacter  | "; break;
            case Action::Type::ChatWord: msg += "| ChatWord       | "; break;
            case Action::Type::CommandBind: msg += "| CommandBind    | "; break;
            case Action::Type::TurnStart: msg += "| TurnStart      | "; break;
            case Action::Type::PushInfo: msg += "| PushInfo       | "; break;
        }
        msg += a.input_;
    }
}

bool wg::GameIO::handle_command(const std::string& command)
{
    if (startswith(command, std::string{"/"}))
    {
        if (startswith(command, "/layout"))
        {
            // Load layout
            const auto fn = wg::get_arg(command);
            wg::log::point("[GameIO] Loading new layout from file: ", fn);

            auto data = wg::filename_to_json(fn);
            if (wg::is_error(data, wg::JSONError::Open))
            {
                chat("Could not open file: " + fn, "Error");  // Error is the author, hm
                return true;
            }
            if (wg::is_error(data, wg::JSONError::Parse))
            {
                chat("Could not parse json in file: " + fn, "Error");  // Oh well
                return true;
            }

            // Note to self:
            // In a future where GameIO does all i/o, including board i/o, there may
            // be a better way to do this. In this case, I've just gone ahead and passed
            // the board directly into the GameIO object.
            assert(data.find("layout") != data.end());
            board_.set_layout(data["layout"]);
            update_handler_.update(wg::ConfUpdate{"layout", data["layout"].dump()});
        }
        else if (startswith(command, "/turn"))
        {
            const auto user = wg::get_arg(command);
            queue_.emplace(wg::Action{Action::Type::TurnStart, user});
        }
        else if (startswith(command, "/charse"))
        {
            // Load charset
            load_charset(wg::get_single_line(wg::get_arg(command)));
        }
        else if (startswith(command, "/charsc"))
        {
            // Load charset
            const auto data = wg::filename_to_json(wg::get_arg(command));
            if (wg::is_error(data, wg::JSONError::Open))
            {
                chat("Could not open file: " + wg::get_arg(command), "Error");
                return true;
            }
            if (wg::is_error(data, wg::JSONError::Parse))
            {
                chat("Could not parse json in file: " + wg::get_arg(command), "Error");
                return true;
            }
            board_.scores_.from_json(data);
            update_handler_.update(wg::ConfUpdate{"charscores", data.dump()});
            return true;
        }
        else if (startswith(command, "/drawtillall"))
        {
            // Draw tiles
            const int num = wg::atoi_default(wg::get_arg(command));
            if (num > hand_.size()) draw_tiles(num - hand_.size());
            update_handler_.update(wg::ConfUpdate{"drawtill", wg::get_arg(command)});
        }
        else if (startswith(command, "/drawtill"))
        {
            // Draw tiles
            const int num = wg::atoi_default(wg::get_arg(command));
            if (num > hand_.size()) draw_tiles(num - hand_.size());
        }
        else if (startswith(command, "/draw"))
        {
            // Draw tiles
            draw_tiles(wg::atoi_default(wg::get_arg(command)));
        }
        else if (startswith(command, "/push"))
        {
            // Push information to others:
            //  - Charset
            update_handler_.update(wg::ConfUpdate{"charset", charset_});
            //  - Player List, etc
            queue_.emplace(wg::Action{Action::Type::PushInfo, ""});
        }
        else if (startswith(command, "/loadscript"))
        {
            // load is instant; loadscript has a more nuanced spec
            // spec... hahaha. more like "more nuanced code-documentation"
            const auto ignore = wg::get_arg(command);
            const std::string filename =
                (wg::has_chars(ignore) ? ignore : /*"load.json"*/ "load.wg");
            wg::log::point("[GameIO] Loading script in file: ", filename);
            const auto v = wg::get_lines(filename, wg::ReadMode::HasChars);
            enum class HeaderMode
            {
                None,
                TurnEnd,
            } header_mode = HeaderMode::None;
            for (const auto& c : v)
            {
                if (startswith(c, "HEADER"))
                {
                    const auto arg = wg::get_arg(c);
                    if (arg == "turn-end")
                    {
                        header_mode = HeaderMode::TurnEnd;
                        break;
                    }
                    else
                    {
                        wg::log::warn("Encountered unknown HEADER declaration: ", arg);
                    }
                }
            }
            switch (header_mode)
            {
                case HeaderMode::TurnEnd:
                {
                    for (const auto& c : v)
                    {
                        if (!startswith(c, "//") && !startswith(c, "#"))
                        {
                            turn_end_script_.push_back(c);
                        }
                    }
                    break;
                }
                default: wg::log::warn("No header specified for script in file: ", filename);
            }
        }
        else if (startswith(command, "/load"))
        {
            // Ideally this file shouldn't be JSON
            const auto ignore = wg::get_arg(command);
            const std::string filename =
                (wg::has_chars(ignore) ? ignore : /*"load.json"*/ "load.wg");
            wg::log::point("[GameIO] Loading new everything from file: ", filename);

            /*
            auto data = wg::filename_to_json(filename);
            if (wg::is_error(data, wg::JSONError::Open))
            {
                chat("Could not open file: " + filename, "Error");  // Error is the author, hm
                return true;
            }
            if (wg::is_error(data, wg::JSONError::Parse))
            {
                chat("Could not parse json in file: " + filename, "Error");  // Oh well
                return true;
            }
            // Okay, we're good
            if (data.find("load") != data.end())
            {
                handle_command("/load=" + data["load"].get<std::string>());
            }
            if (data.find("charset") != data.end())
            {
                handle_command("/charset=" + data["charset"].get<std::string>());
            }
            */
            for (const auto& c : wg::get_lines(filename, wg::ReadMode::HasChars))
            {
                if (!startswith(c, "//") && !startswith(c, "#"))
                {
                    wg::log::point("Handling command: ", c);
                    handle_command(c);
                }
            }
        }
        else
        {
            wg::log::warn("Command does not exist: ", command);
        }
        return true;
    }
    if (startswith(command, std::string{"?"}))
    {
        return true;
    }
    return false;
}

void wg::GameIO::load_charset(std::string charset)
{
    charset_ = std::move(charset);
    update_handler_.update(wg::ConfUpdate{"charset", charset_});
}

void wg::GameIO::draw_tiles(int num)
{
    if (num < 1) return;
    hand_ += wg::join(wg::select_randomly_destructively(charset_, num));
    hand_text_.setString("Hand: " + hand_);
    update_handler_.update(wg::ConfUpdate{"charset", charset_});
}

void wg::GameIO::setup_text(sf::Text& text, const std::string& contents)
{
    text.setFont(manager_.defaultFont()->font);
    text.setCharacterSize(default_character_size_);
    text.setFillColor(sf::Color::Black);
    text.setString(contents);
}

void wg::GameIO::position_playerlist(sf::Transformable& l)
{
    l.setPosition(target_.width() / 2 + 50, target_.height() - 200);
}

void wg::GameIO::end_turn()
{
    for (const auto& v : turn_end_script_) handle_command(v);
}
