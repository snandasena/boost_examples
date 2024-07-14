//
// Created by sajith.nandasena on 08.07.2024.
//
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>


int main()
{
    net::io_context io_context;

    tcp::resolver resolver{io_context};
    websocket::stream<tcp::socket> ws{io_context};


    const std::string host{"0.0.0.0"};
    const std::string port{"3000"};

    const auto results = resolver.resolve(host, port);

    const std::string address{host + ":" + port};

    ws.set_option(websocket::stream_base::decorator([](websocket::request_type &req)
    {
        req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-sync");
    }));

    ws.handshake(address, "/");

    ws.write(net::buffer(std::string("Hello from client")));

    beast::flat_buffer buffer;
    ws.read(buffer);

    ws.close(websocket::close_code::normal);

    std::cout << beast::make_printable(buffer.data()) << std::endl;
}
