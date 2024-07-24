//
// Created by sajith.nandasena on 18.07.2024.
//

#ifndef SESSION_H
#define SESSION_H

#include <utils/common.h>


class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<ssl::stream<beast::tcp_stream> > ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string text_;

public:
    session(net::io_context &ioc, ssl::context &ctx);

    void run(char const *host, char const *port, char const *text);

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);

    void on_ssl_handshake(beast::error_code ec);

    void on_handshake(beast::error_code ec);

    void on_write(beast::error_code ec, std::size_t bytes_transferred);

    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    void on_close(beast::error_code ec);
};

#endif //SESSION_H
