//
// Created by sajith.nandasena on 16.07.2024.
//

#ifndef LISTENER_H
#define LISTENER_H

#include <utils/common.h>

class listener : public std::enable_shared_from_this<listener>
{
    net::io_context &ioc_;
    ssl::context &ctx_;
    tcp::acceptor acceptor_;

public:
    listener(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint);

    void run();

private:
    void do_accept();

    void on_accept(beast::error_code ec, tcp::socket socket);
};

#endif //LISTENER_H
