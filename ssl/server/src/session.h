//
// Created by sajith.nandasena on 16.07.2024.
//

#ifndef SESSION_H
#define SESSION_H

#include "common.h"

class session : public std::enable_shared_from_this<session>
{
    websocket::stream<ssl::stream<beast::tcp_stream> > ws_;
    beast::flat_buffer buffer_;

public:
    session(tcp::socket &&socket, ssl::context &ctx);

    void run();

    void on_run();

    void on_handshake(beast::error_code ec);

    void on_accept(beast::error_code ec);

    void do_read();

    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    void on_write(beast::error_code ec, std::size_t byte_transferred);
};

#endif //SESSION_H
