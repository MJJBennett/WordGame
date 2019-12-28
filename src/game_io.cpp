#include "game/game_io.hpp"

#include "debug/log.hpp"
#include "framework/resource.hpp"
#include "framework/resourcemanager.hpp"
#include "framework/tools.hpp"
#include "framework/window_context.hpp"
#include "framework/window_io.hpp"
#include "update_handler.hpp"

// This is basically just meant to be a better version of the
// 'window io' static functions.

wg::GameIO::GameIO(wg::WindowContext& target, wg::ResourceManager& manager,
                   wg::UpdateHandler& update_handler)
    : target_(target), manager_(manager), update_handler_(update_handler)
{
    chat_text_.setFont(manager.defaultFont()->font);
    chat_text_.setCharacterSize(message_character_size_);
    chat_text_.setFillColor(sf::Color::Black);
    chat_text_.setPosition(message_left_offset_, target.height() - message_bar_height_);
}

void wg::GameIO::init()
{
    user_ = wg::window_io::get_from_file(target_, manager_, "Enter Username", "name.txt");
}

bool wg::GameIO::do_event(const sf::Event& e)
{
    switch (e.type)
    {
        case sf::Event::TextEntered: text_entered(e.text.unicode); break;
        case sf::Event::KeyPressed: key_pressed(e.key.code); break;
        case sf::Event::KeyReleased: key_released(e.key.code); break;
        default: return false;
    }
    return true;
}

void wg::GameIO::text_entered(unsigned int c)
{
    // Where do we redirect this?
    // To the board, if we're in 'Board Edit' mode...
    if (mode_ == Mode::BoardEdit)
    {
        auto ch = get_char<std::optional<char>>(c);
        if (ch) queue_.push({Action::Type::BoardCharacter, std::string{*ch}});
        return;
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
    }
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
    chat_message.setPosition(message_left_offset_, target_.height() - message_bar_height_ - message_bar_height_);
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

void wg::GameIO::key_pressed(sf::Keyboard::Key k)
{
    switch (k)
    {
        case sf::Keyboard::Key::Enter: return do_enter();
        default: return;
    }
}

void wg::GameIO::key_released(sf::Keyboard::Key k)
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
            return;  // this is done on keypress, see do_enter
        }
        default: return;
    }
}

void wg::GameIO::do_enter()
{
    switch (mode_)
    {
        case Mode::Normal:
        {
            mode_ = Mode::ChatEdit;
            return;
        }
        case Mode::BoardEdit:
        {
            mode_ = Mode::Normal;
            return;
        }
        case Mode::ChatEdit:
        {
            if (partial_action_ && partial_action_->type_ == Action::Type::ChatWord &&
                (*partial_action_).input_.length() > 0)
            {
                if (!has_chars((*partial_action_).input_))
                {
                    // clear input
                }
                else
                {
                    wg::log::point("Sending message: ", (*partial_action_).input_);
                    chat_broadcast((*partial_action_).input_, user_);
                    queue_.push(*partial_action_); // Record action
                }
                chat_text_.setString("");
                partial_action_.reset();
            }
            mode_ = Mode::Normal;
            return;
        }
    }
}

void wg::GameIO::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(chat_text_);
    for (auto&& t : chat_bar_) target.draw(t);
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
        }
        msg += a.input_;
    }
}
