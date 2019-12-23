#include "ws_session.hpp"

#include <boost/beast/core/buffers_to_string.hpp>
#include <nlohmann/json.hpp>
#include "debug/log.hpp"

namespace websocket = boost::beast::websocket;
using tcp           = boost::asio::ip::tcp;

wg::WebSocketSession::WebSocketSession(tcp::socket&& socket) : websocket_(std::move(socket))
{
    wg::log::point("[WebSocket] Created a websocket session.");
}

void wg::WebSocketSession::launch(http::request<http::string_body> req)
{
    websocket_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    websocket_.set_option(websocket::stream_base::decorator([](websocket::response_type& resp) {
        resp.set(http::field::server,
                 std::string{BOOST_BEAST_VERSION_STRING} + " websocket-wordgame");
    }));

    websocket_.async_accept(
        req, beast::bind_front_handler(&WebSocketSession::on_accept, shared_from_this()));
}

void wg::WebSocketSession::on_accept(beast::error_code ec)
{
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
            wg::log::err("[WebSocket] Failed to accept: ", ec.message());
        return;
    }

    // might want to track that this is an active session here

    websocket_.async_read(
        buffer_, beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
}

void wg::WebSocketSession::on_read(beast::error_code ec, std::size_t)
{
    if (ec)
    {
        if (ec == asio::error::operation_aborted) return;
        if (ec == websocket::error::closed)
        {
            wg::log::point("[WebSocket] The remote connection has been closed.");
            return;
        }
        wg::log::err("[WebSocket] Failed to read: ", ec.message());
        return;
    }

    const auto msg = beast::buffers_to_string(buffer_.data());
    wg::log::data("Received message", msg);
    auto data     = nlohmann::json::parse(msg);
    const int seq = data["seq"];
    if (rem_seq_ == 0)
        rem_seq_ = seq;
    else if (rem_seq_ + 1 != seq)
        wg::log::warn("[WebSocket] Received sequence number ", seq, " when ", rem_seq_ + 1,
                      " was expected.");
    rem_seq_                  = seq;
    const std::string payload = data["msg"];
    const auto ret            = nlohmann::json{{"type", "ack"}, {"ack", seq + 1}};

    buffer_.consume(buffer_.size());

    write_queue_.push(ret.dump());
    if (write_queue_.size() == 1)
    {
        wg::log::point("[WebSocket] Launching write.");
        websocket_.async_write(
            asio::buffer(write_queue_.front()),
            beast::bind_front_handler(&WebSocketSession::on_write, shared_from_this()));
    }

    websocket_.async_read(
        buffer_, beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
}

void wg::WebSocketSession::on_write(beast::error_code ec, std::size_t)
{
    if (wg::log::opt_err(ec, "[WebSocket] Failed to write")) return;

    wg::log::point("[WebSocket] Successfully completed write.");
    write_queue_.pop();

    if (write_queue_.empty()) return;
    wg::log::point("[WebSocket] Launching write.");
    websocket_.async_write(
        asio::buffer(write_queue_.front()),
        beast::bind_front_handler(&WebSocketSession::on_write, shared_from_this()));
}

wg::WebSocketSession::~WebSocketSession()
{
    wg::log::point("[WebSocket] Destroyed a websocket session.");
}

void wg::WebSocketSession::shutdown()
{
    // We're declining the end user (in an unfriendly fashion)
    wg::log::warn("[WebSocket] Shutting down websocket session from the server.");
    beast::get_lowest_layer(websocket_).cancel();
}