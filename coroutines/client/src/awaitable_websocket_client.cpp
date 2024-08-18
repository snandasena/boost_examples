//
// Created by sajit on 18/08/2024.
//

#include <utils/common.h>


net::awaitable<void> do_session(std::string host, std::string port, std::string text)
{
    auto executor = co_await net::this_coro::executor;
    auto resolver = net::ip::tcp::resolver{executor};
    auto stream = websocket::stream<beast::tcp_stream>{executor};

    const auto results =
            co_await resolver.async_resolve(host, port, net::use_awaitable);

    beast::get_lowest_layer(stream).expires_after(std::chrono::seconds{30});

    auto ep = co_await beast::get_lowest_layer(stream).async_connect(results, net::use_awaitable);

    host += ':' + std::to_string(ep.port());

    beast::get_lowest_layer(stream).expires_never();

    stream.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    stream.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req)
            {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coroutine");
            }));

    co_await stream.async_handshake(host, "/", net::use_awaitable);

    co_await stream.async_write(net::buffer(text), net::use_awaitable);

    beast::flat_buffer buffer;

    co_await stream.async_read(buffer, net::use_awaitable);

    co_await stream.async_close(websocket::close_code::normal, net::use_awaitable);

    std::cout << "Response: " << beast::make_printable(buffer.data()) << std::endl;
}


int main()
{
    const auto host = "0.0.0.0";
    const auto port = "3000";
    const auto text = "Hello text from the client";


    try
    {
        net::io_context ioc;
        net::co_spawn(
                ioc,
                do_session(host, port, text),
                [](std::exception_ptr ptr)
                {

                    if (ptr)
                        std::rethrow_exception(ptr);
                }
        );

        ioc.run();
    } catch (const std::exception &e)
    {
        std::cerr << "Error " << e.what() << std::endl;
    }
    return 0;
}