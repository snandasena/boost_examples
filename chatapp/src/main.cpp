//
// Created by sajith.nandasena on 10.07.2024.
//

#include "listener.h"
#include "shared_state.h"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>


int main()
{
    net::io_context ioc;
    const auto adress = net::ip::make_address("0.0.0.0");
    const auto port = 3000u;
    const auto doc_root = "";

    boost::make_shared<listener>(ioc, tcp::endpoint{adress, port}, boost::make_shared<shared_state>(doc_root))->run();

    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](boost::system::error_code const &, int)
    {
        ioc.stop();
    });

    std::vector<std::thread> v;
    const auto threads = 4;

    v.reserve(threads - 1);

    for (auto i = threads - 1; i > 0; --i)
    {
        v.emplace_back([&ioc]()
        {
            ioc.run();
        });
    }

    ioc.run();

    for (auto &t: v)
    {
        t.join();
    }

    return 0;
}
