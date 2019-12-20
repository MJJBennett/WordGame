#include "client.hpp"

#include "debug/log.hpp"

wg::WebSocketClient::WebSocketClient() : resolver_(ioc_), ws_(ioc_)
{
    wg::log::point("Constructing websocket wrapper.");
    boost::beast::error_code ec{};

#include "VERYTEMP_remoteaddr.tmp"

    wg::log::point("Connecting to websocket at URL: ", remote_address, " and port: ", 27600);

    wg::log::point("Resolving websocket URL.");
    auto const results = resolver_.resolve(remote_address, "27600");

    wg::log::point("Connecting to the IP address using Asio.");
    beast::get_lowest_layer(ws_).connect(results, ec);
    if (ec)
    {
        wg::log::err("[Client] Error connecting: ", ec.message());
        return;
    }

    // Timeout in websocket, not in tcp
    beast::get_lowest_layer(ws_).expires_never();

    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
        req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) + " websocket-wordgame-client");
    }));

    wg::log::point("Performing the websocket handshake.");
    ws_.handshake(remote_address, std::string("/"));
    wg::log::point("Successfully performed the handshake! Waiting for launch() call.");
}

void wg::WebSocketClient::on_write(beast::error_code ec, std::size_t)
{
    if (ec)
    {
        wg::log::err("[WebSocketClient] Write failed: ", ec.message());
        return;
    }

    // done for now, just testing
}

void wg::WebSocketClient::launch()
{
    // note: cannot call shared_from_this in the constructor
    // today, I have learned something new! (heh, memory pun)
    ws_.async_write(asio::buffer(persistent_str_),
        beast::bind_front_handler(&WebSocketClient::on_write, shared_from_this()));

    ioc_.run();
}

void wg::WebSocketClient::shutdown()
{
    try
    {
        ws_.close(websocket::close_code::normal);
    }
    catch (const std::exception& e)
    {
        wg::log::err("[Client (", __func__, ")] ", e.what());
    }
}
