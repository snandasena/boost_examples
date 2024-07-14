//
// Created by sajith.nandasena on 08.07.2024.
//


#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>


void do_session(tcp::socket socket);

int main()
{
    const auto address = net::ip::make_address("0.0.0.0");
    const unsigned port = 3000;

    net::io_context io_context{1};

    tcp::acceptor acceptor{io_context, {address, port}};

    for (;;)
    {
        tcp::socket socket{io_context};
        acceptor.accept(socket);

        std::thread(&do_session, std::move(socket)).detach();
    }
}

void do_session(tcp::socket socket)
{
    try
    {
        websocket::stream<tcp::socket> ws{std::move(socket)};

        ws.set_option(websocket::stream_base::decorator([](websocket::response_type &res)
        {
            res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) + "web socker server sync");
        }));

        ws.accept();

        for (;;)
        {
            beast::flat_buffer buffer;
            ws.read(buffer);

            std::cout << beast::make_printable(buffer.data()) << std::endl;

            ws.text(ws.got_text());
            ws.write(buffer.data());
        }
    } catch (beast::system_error const &se)
    {
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
