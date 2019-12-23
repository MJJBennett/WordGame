#include "client.hpp"

#include <nlohmann/json.hpp>
#include <random>
#include "debug/log.hpp"

wg::WebSocketClient::WebSocketClient(std::string target, std::string port)
    : resolver_(ioc_), ws_(ioc_), target_(std::move(target)), port_(std::move(port))
{
    // Random sequence number, temporary I hope
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> dist(314, 728);
    seq_ = dist(generator);

    wg::log::point("[Client] Constructed a websocket wrapper.");
}

wg::WebSocketClient::~WebSocketClient()
{
    if (status_ != Status::Closed) shutdown();
}

// Client entry point - blocking.
void wg::WebSocketClient::launch()
{
    // note: you cannot call shared_from_this in the constructor
    // today, I have learned something new! (heh, memory pun)
    wg::log::point("[Client] Connecting to websocket at URL: ", target_, " and port: ", port_,
                   "\n\tResolving websocket URL...");

    // Queue our first async operation, so our ioc_.run() call doesn't
    // immediately exit after not having anything to do.
    resolver_.async_resolve(
        target_, port_,
        beast::bind_front_handler(&WebSocketClient::on_resolve, shared_from_this()));

    ioc_.run();
}

void wg::WebSocketClient::on_resolve(beast::error_code ec, resolver::results_type results)
{
    if (wg::log::opt_err(ec, "[Client] Resolve failed")) return;

    wg::log::point("[Client] Connecting to the IP address...");
    beast::get_lowest_layer(ws_).async_connect(
        results, beast::bind_front_handler(&WebSocketClient::on_connect, shared_from_this()));
}

void wg::WebSocketClient::on_connect(beast::error_code ec,
                                     resolver::results_type::endpoint_type endpoint)
{
    if (wg::log::opt_err(ec, "[Client] Connect failed")) return;

    // Timeout in websocket, not in tcp
    beast::get_lowest_layer(ws_).expires_never();

    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
        req.set(beast::http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) + " websocket-wordgame-client");
    }));

    wg::log::point("[Client] Performing the websocket handshake...");
    ws_.async_handshake(
        target_, std::string("/"),
        beast::bind_front_handler(&WebSocketClient::on_handshake, shared_from_this()));
}

void wg::WebSocketClient::on_handshake(beast::error_code ec)
{
    if (wg::log::opt_err(ec, "[Client] Handshake failed")) return;

    // Now we need to authenticate with the server.

    ws_.async_write(asio::buffer(format_message(*message_)),
                    beast::bind_front_handler(&WebSocketClient::on_write, shared_from_this()));

    ws_.async_read(buffer_, std::bind(&WebSocketClient::on_read, shared_from_this(),
                                      std::placeholders::_1, std::placeholders::_2));
}

void wg::WebSocketClient::on_write(beast::error_code ec, std::size_t)
{
    if (wg::log::opt_err(ec, "[Client] Write failed")) return;

    message_.reset();

    // Simple queueing system for setup and testing
    if (message_queue_.size())
    {
        message_ = message_queue_.front();
        message_queue_.pop();
        ws_.async_write(asio::buffer(format_message(*message_)),
                        beast::bind_front_handler(&WebSocketClient::on_write, shared_from_this()));
    }
}

void wg::WebSocketClient::on_read(beast::error_code ec, std::size_t)
{
    if (wg::log::opt_err(ec, "[Client] Read failed")) return;

    auto str = parse_message(beast::buffers_to_string(buffer_.data()));

    buffer_.consume(buffer_.size());

    if (str) {  // This could probably be its own function, really
        const std::lock_guard<std::mutex> lock(recv_mutex_);
        recv_queue_.push(std::move(*str));
    }

    // Launch another async read!
    ws_.async_read(buffer_, std::bind(&WebSocketClient::on_read, shared_from_this(),
                                      std::placeholders::_1, std::placeholders::_2));
}

void wg::WebSocketClient::shutdown()
{
    wg::log::point("[Client] Shutting down web client.");
    if (status_ == Status::Closed)
    {
        wg::log::warn("[Client] Web socket was already closed.");
        return;
    }
    try
    {
        ws_.close(websocket::close_code::normal);
        status_ = Status::Closed;
    }
    catch (const std::exception& e)
    {
        wg::log::err("[Client (", __func__, ")] ", e.what());
    }
}

void wg::WebSocketClient::queue_send(std::string message)
{
    asio::post(ioc_, beast::bind_front_handler(&WebSocketClient::send, shared_from_this(),
                                               std::move(message)));
}

void wg::WebSocketClient::queue_shutdown()
{
    asio::post(ioc_, std::bind(&WebSocketClient::shutdown, shared_from_this()));
}

std::string wg::WebSocketClient::format_message(std::string message)
{
    const auto data = nlohmann::json{{"type", "msg"}, {"msg", std::move(message)}, {"seq", seq_++}};
    return data.dump();
}

std::optional<std::string> wg::WebSocketClient::parse_message(std::string message)
{
    const auto data = nlohmann::json::parse(message);
    if (data["type"] != "ack") return data["msg"];
    return {};
}

// Launches async operation - async_write
void wg::WebSocketClient::send(std::string message)
{
    // Push our message to the end of the queue
    message_queue_.push(message);

    // This is only the case if we're already in a async_write!
    if (message_) return;

    // Great, no write currently happening
    message_ = message_queue_.front();
    message_queue_.pop();
    ws_.async_write(asio::buffer(format_message(*message_)),
                    beast::bind_front_handler(&WebSocketClient::on_write, shared_from_this()));
}

size_t wg::WebSocketClient::num_waiting()
{
    const std::lock_guard<std::mutex> lock(recv_mutex_);
    return recv_queue_.size();
}

std::optional<std::string> wg::WebSocketClient::read_once()
{
    const std::lock_guard<std::mutex> lock(recv_mutex_);
    if (recv_queue_.size() == 0) return {};
    const auto str(std::move(recv_queue_.front()));
    recv_queue_.pop();
    return str;
}

std::queue<std::string> wg::WebSocketClient::read_all()
{
    const std::lock_guard<std::mutex> lock(recv_mutex_);
    std::queue<std::string> q;
    q.swap(recv_queue_);
    return q;
}
