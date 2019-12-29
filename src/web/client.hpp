#ifndef WG_WEBSOCKET_CLIENT_HPP
#define WG_WEBSOCKET_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <mutex>

namespace wg
{
namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace asio      = boost::asio;
using resolver      = asio::ip::tcp::resolver;

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
    WebSocketClient(std::string target, std::string port = "27600");
    ~WebSocketClient();

    // Blocking call, launches the websocket
    void launch();
    // Threadsafe shutdown
    void queue_shutdown();

    // Sends a single string message over the connection
    void queue_send(std::string message);

    size_t num_waiting();
    std::optional<std::string> read_once();
    std::queue<std::string> read_all();

    std::string format_message(std::string);
    std::optional<std::string> parse_message(std::string);

private:
    // Async handlers
    void on_resolve(beast::error_code, resolver::results_type);
    void on_connect(beast::error_code, resolver::results_type::endpoint_type);
    void on_handshake(beast::error_code);
    void on_write(beast::error_code, std::size_t);
    void on_read(beast::error_code, std::size_t);

    void send(std::string message);

    void shutdown();

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
    std::optional<std::string> message_{""};

    // Received queue - messages waiting to be read
    std::queue<std::string> recv_queue_;
    // Mutex for threadsafe queue access
    std::mutex recv_mutex_;

    unsigned int seq_{0};

    // Read message will be here
    beast::flat_buffer buffer_;

    enum class Status
    {
        Ready,
        Running,
        Closed
    } status_ = Status::Ready;
};
}  // namespace wg

#endif  // WG_WEBSOCKET_CLIENT_HPP
