//
// Created by sajith.nandasena on 11.07.2024.
//

#ifndef LISTENER_H
#define LISTENER_H


#include "beast.h"
#include "net.h"
#include <boost/smart_ptr.hpp>
#include <memory>
#include <string>


class shared_state;


class listener : public boost::enable_shared_from_this<listener>
{
    net::io_context &ioc_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<shared_state> state_;

    void fail(beast::error_code ec, char const *what);

    void on_accept(beast::error_code ec, tcp::socket socket);

public:
    listener(net::io_context &ioc, tcp::endpoint endpoint, boost::shared_ptr<shared_state> const &state);

    void run();
};

#endif //LISTENER_H
