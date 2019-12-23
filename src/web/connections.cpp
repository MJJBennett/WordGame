#include "connections.hpp"

#include "wg_session.hpp"

void wg::Connections::alert_connections(WebSocketSession* ignore, std::string message)
{
    for (auto&& ptr : connections_)
    {
        if (ptr == ignore) continue;
        ptr->queue_write(message);
    }
}
