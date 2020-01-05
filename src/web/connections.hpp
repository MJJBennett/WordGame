#ifndef WG_CONNECTIONS_HPP
#define WG_CONNECTIONS_HPP

#include <string>
#include <unordered_map>

namespace wg
{
class WebSocketSession;

class Connections
{
public:
    void add_connection(WebSocketSession* ptr) { connections_[ptr] = ""; }
    void remove_connection(WebSocketSession* ptr);
    bool claim_host(WebSocketSession* ptr)
    {
        if (host_ == nullptr)
        {
            host_ = ptr;
            return true;
        }
        return false;
    }
    bool is_host(WebSocketSession* ptr) { return ptr == host_; }

    void set_user(WebSocketSession* ptr, const std::string& name) { connections_.at(ptr) = name; }

    // okay, the example method of doing this is way more efficient
    // easy fix to this would be to just make std::string a shared pointer,
    // or a reference
    void alert_connections(WebSocketSession* ignore, std::string message);

private:
    std::unordered_map<WebSocketSession*, std::string> connections_;
    WebSocketSession* host_{nullptr};
};
}  // namespace wg

#endif  // WG_CONNECTIONS_HPP
