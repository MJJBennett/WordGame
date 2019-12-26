#ifndef WG_UPDATE_HANDLER_HPP
#define WG_UPDATE_HANDLER_HPP

#include "game/update.hpp"

namespace wg
{
class UpdateHandler
{
public:
    virtual void update(const wg::GameUpdate& u) = 0;
    virtual void update(const wg::ChatUpdate& u) = 0;
};
}  // namespace wg

#endif  // WG_UPDATE_HANDLER_HPP
