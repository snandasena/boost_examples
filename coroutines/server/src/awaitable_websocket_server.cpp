//
// Created by sajit on 18/08/2024.
//

#include <utils/common.h>

net::awaitable<void> do_session(websocket::stream<beast::tcp_stream> stream)
{
    stream.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    stream.set_option(websocket::stream_base::decorator(
            [](websocket::response_type &res)
            {
                res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server");
            }));

    co_await stream.async_accept(net::use_awaitable);

    for (;;)
    {
        beast::flat_buffer buffer;

        auto [ec, _] = co_await stream.async_read(buffer, net::as_tuple(net::use_awaitable));

        if (ec == websocket::error::closed)
            co_return;

        if (ec)
            throw boost::system::system_error{ec};

        std::cout << "Received message: " << beast::make_printable(buffer.data()) << std::endl;

        stream.text(stream.got_text());
        co_await stream.async_write(buffer.data(), net::use_awaitable);
    }
}

net::awaitable<void> do_listen(net::ip::tcp::endpoint endpoint)
{
    auto executor = co_await net::this_coro::executor;
    auto acceptor = net::ip::tcp::acceptor{executor, endpoint};

    for (;;)
    {
        net::co_spawn(
                executor,
                do_session(
                        websocket::stream<beast::tcp_stream>{
                                co_await acceptor.async_accept(net::use_awaitable)}),
                [](std::exception_ptr ptr)
                {
                    if (ptr)
                    {
                        try
                        {
                            std::rethrow_exception(ptr);
                        } catch (const std::exception &e)
                        {
                            std::cerr << "Error in session " << e.what() << std::endl;
                        }
                    }
                });
    }
}


int main()
{

    const auto address = net::ip::make_address("0.0.0.0");
    const auto port = 3000u;

    net::io_context ioc{};

    net::co_spawn(ioc,
                  do_listen(net::ip::tcp::endpoint{address, port}),
                  [](std::exception_ptr e)
                  {
                      if (e)
                      {
                          try
                          {
                              std::rethrow_exception(e);
                          } catch (const std::exception &e)
                          {
                              std::cerr << "Error " << e.what() << std::endl;
                          }
                      }
                  });

    ioc.run();

    return 0;
}