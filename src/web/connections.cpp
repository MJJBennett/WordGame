#include "connections.hpp"

#include "ws_session.hpp"

void wg::Connections::alert_connections(WebSocketSession* ignore, std::string message)
{
    for (auto&& ptr : connections_)
    {
        if (ptr.first == ignore) continue;
        ptr.first->queue_write(message);
    }
}

void wg::Connections::remove_connection(WebSocketSession* to_disconnect)
{
    const auto str = connections_[to_disconnect];
    for (auto&& ptr : connections_)
    {
        if (ptr.first == to_disconnect) continue;
        ptr.first->queue_disconnect(str);
    }
    connections_.erase(to_disconnect);
    if (host_ == to_disconnect) host_ = nullptr;
}

