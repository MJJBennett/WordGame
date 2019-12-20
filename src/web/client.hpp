#ifndef WG_WEBSOCKET_CLIENT_HPP
#define WG_WEBSOCKET_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <optional>
#include <queue>

namespace wg
{
namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace asio      = boost::asio;

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
    WebSocketClient(std::string target, std::string port = "27600");

    // Blocking call, launches the websocket
    void launch();
    // Closes the websocket
    void shutdown();

    // Sends a single string message over the connection
    void send(std::string message);

private:
    // Async handlers
    void on_write(beast::error_code, std::size_t);

private:
    // The client has its own io context
    asio::io_context ioc_;
    asio::ip::tcp::resolver resolver_;

    // The core of this class - a wrapped websocket stream
    websocket::stream<beast::tcp_stream> ws_;

    // Target remote machine to connect to
    std::string target_;
    std::string port_;

    // Message queue - messages that still need to be sent
    std::queue<std::string> message_queue_;
    // Current message waiting to be sent.
    std::optional<std::string> message_{"{\"seq\":314}"};

    enum class Status
    {
        Ready,
        Running,
        Closed
    } status_ = Status::Ready;
};
}  // namespace wg

#endif  // WG_WEBSOCKET_CLIENT_HPP
