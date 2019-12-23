#ifndef WG_CONNECTIONS_HPP
#define WG_CONNECTIONS_HPP

#include <string>
#include <unordered_set>

namespace wg
{
class WebSocketSession;

class Connections
{
public:
    void add_connection(WebSocketSession* ptr) { connections_.insert(ptr); }
    void remove_connection(WebSocketSession* ptr) { connections_.erase(ptr); }

    // okay, the example method of doing this is way more efficient
    // easy fix to this would be to just make std::string a shared pointer,
    // or a reference
    void alert_connections(WebSocketSession* ignore, std::string message);

private:
    std::unordered_set<WebSocketSession*> connections_;
};
}  // namespace wg

#endif  // WG_CONNECTIONS_HPP
