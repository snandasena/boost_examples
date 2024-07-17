//
// Created by sajith.nandasena on 16.07.2024.
//

#include "listener.h"

listener::listener(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint): ioc_(ioc), ctx_(ctx),
    acceptor_(net::make_strand(ioc))
{
}
