#include "session.hpp"

#include "debug/log.hpp"
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include "ws_session.hpp"

using tcp           = boost::asio::ip::tcp;

wg::Session::Session(boost::asio::ip::tcp::socket&& socket)
    : tcp_stream_(std::move(socket))
{
    wg::log::point("[Session] Created a server session.");
}

wg::Session::~Session() { wg::log::point("[Session] Destroyed a server session."); }

void wg::Session::launch()
{
    wg::log::point("[Session] Launching dedicated server session.");
    start_read();
}

void wg::Session::start_read()
{
    wg::log::point("[Session] Setting up read.");
    // Clear the request parser while we have free time
    req_parser_.emplace();

    req_parser_->body_limit(4096);

    tcp_stream_.expires_after(std::chrono::minutes(2));

    wg::log::point("[Session] Launching read.");
    http::async_read(tcp_stream_, buffer_, req_parser_->get(),
                     beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void wg::Session::on_read(beast::error_code ec, std::size_t)
{
    wg::log::point("[Session] Reading message.");
    if (ec == http::error::end_of_stream)
    {
        tcp_stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        return;
    }
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
            wg::log::err("Could not complete a read: ", ec.message());
        return;
    }

    if (beast::websocket::is_upgrade(req_parser_->get()))
    {
        wg::log::point("[Session] Message was upgrade: Upgrading session to WebSocket.");
        // We're upgrading to a websocket connection with the remote client
        // Hooray!
        std::make_shared<wg::WebSocketSession>(tcp_stream_.release_socket())
            ->launch(req_parser_->release());
    }
}

void wg::Session::on_write(beast::error_code, std::size_t, bool) {}
