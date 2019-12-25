#ifndef WG_GAME_IO_HPP
#define WG_GAME_IO_HPP

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
    } type_;

    std::string input_;
};

class GameIO
{
public:
    GameIO();

private:
};
}  // namespace wg

#endif  // WG_GAME_IO_HPP
