#include "server.hpp"

#include <boost/beast.hpp>
#include "debug/log.hpp"

using tcp           = boost::asio::ip::tcp;
namespace asio      = boost::asio;
namespace beast     = boost::beast;
namespace websocket = boost::beast::websocket;

wg::Server::Server() : tcp_acceptor_(ioc_)
{
    wg::log::point("Initializing webserver.");

    boost::beast::error_code ec;

    signals_.async_wait([&](boost::system::error_code const&, int) { this->ioc_.stop(); });

    // { address, port }
#include "VERYTEMP_address.tmp"
    const auto port = 27600;
    wg::log::point("[Server] Running on: ", address, ':', port);
    auto endpoint = tcp::endpoint{boost::asio::ip::make_address(address), port};
    tcp_acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
            wg::log::err("Could not open a socket: ", ec.message());
        return;
    }

    tcp_acceptor_.bind(endpoint, ec);
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
            wg::log::err("Could not bind a socket: ", ec.message());
        return;
    }

    tcp_acceptor_.listen(asio::socket_base::max_listen_connections, ec);
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
            wg::log::err("Could not listen on the socket: ", ec.message());
        return;
    }

    wg::log::point("Successfully configured webserver.");

    tcp_acceptor_.async_accept(asio::make_strand(ioc_),
                               beast::bind_front_handler(&Server::on_accept, this));

    wg::log::point("Launching webserver!");
    ioc_.run();
}

void wg::Server::on_accept(boost::system::error_code const& ec, tcp::socket socket)
{
    wg::log::point("[Server] Initiating acceptance.");
    if (ec)
    {
        if (ec != asio::error::operation_aborted)
            wg::log::err("Could not accept a connection: ", ec.message());
        return;
    }

    wg::log::point("> Launching server session.");
    boost::make_shared<Server::Session>(std::move(socket))->launch();

    wg::log::point("> Continuing to accept connections.");
    tcp_acceptor_.async_accept(asio::make_strand(ioc_),
                               beast::bind_front_handler(&Server::on_accept, this));
}

wg::Server::Session::Session(boost::asio::ip::tcp::socket&& socket)
    : tcp_stream_(std::move(socket))
{
    wg::log::point("[Session] Created a server session.");
}

wg::Server::Session::~Session() { wg::log::point("[Session] Destroyed a server session."); }

void wg::Server::Session::launch()
{
    wg::log::point("[Session] Launching dedicated server session.");
    start_read();
}

void wg::Server::Session::start_read()
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

void wg::Server::Session::on_read(beast::error_code ec, std::size_t)
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
        boost::make_shared<WebSocketSession>(tcp_stream_.release_socket())
            ->launch(req_parser_->release());
    }
}

void wg::Server::Session::on_write(beast::error_code, std::size_t, bool) {}

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
        if (ec != asio::error::operation_aborted)
            wg::log::err("[WebSocket] Failed to read: ", ec.message());
        return;
    }

    wg::log::data("Received message", beast::buffers_to_string(buffer_.data()));

    buffer_.consume(buffer_.size());

    websocket_.async_read(
        buffer_, beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
}
