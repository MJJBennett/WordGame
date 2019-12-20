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
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace asio      = boost::asio;
using tcp           = boost::asio::ip::tcp;

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
    WebSocketClient();

    // Blocking call, launches the web socket
    void launch();
    void shutdown();

    void on_write(beast::error_code, std::size_t);

private:
    // The client has its own io context
    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::resolver resolver_;

    // The core of this class - a wrapped websocket stream
    websocket::stream<beast::tcp_stream> ws_;

    std::queue<std::string> message_queue_;
    std::optional<std::string> message_{"Zero string!"};

    enum class Status
    {
        Ready,
        Running,
        Closed
    } status_ = Status::Ready;
};
}  // namespace wg

#endif  // WG_WEBSOCKET_CLIENT_HPP
