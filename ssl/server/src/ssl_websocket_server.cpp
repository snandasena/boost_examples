//
// Created by sajith.nandasena on 16.07.2024.
//

#include "listener.h"

#include <certs/server_certificate.h>


int main()
{
    const auto address = net::ip::make_address("0.0.0.0");
    const auto port = 3000u;
    const auto threads = 4;

    net::io_context ioc{threads};

    ssl::context ctx{ssl::context::tlsv13};

    load_server_certificate(ctx);

    std::make_shared<listener>(ioc, ctx, tcp::endpoint{address, port})->run();

    std::vector<std::thread> v;
    v.reserve(threads - 1);

    for (auto i = threads - 1; i > 0; --i)
    {
        v.emplace_back([&ioc]()
                       {
                           ioc.run();
                       });
    }

    ioc.run();

    return 0;

}
