//
// Created by sajith.nandasena on 18.07.2024.
//

#include "session.h"

#include <utils/error.h>
#include <utils/ignore_unused.h>


session::session(net::io_context &ioc, ssl::context &ctx) : resolver_(net::make_strand(ioc)),
                                                            ws_(net::make_strand(ioc), ctx)
{
}


void session::run(char const *host, char const *port, char const *text)
{
    host_ = host;
    text_ = text;

    resolver_.async_resolve(host, port,
                            beast::bind_front_handler(&session::on_resolve, shared_from_this()));
}


void session::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
        return fail(ec, "resolve");

    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds{30});

    beast::get_lowest_layer(ws_)
            .async_connect(results,
                           beast::bind_front_handler(
                                   &session::on_connect,
                                   shared_from_this()));
}

void session::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
        return fail(ec, "connect");

    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds{30});

    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str()))
    {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        return fail(ec, "connect");
    }

    host_ += ':' + std::to_string(ep.port());

    ws_.next_layer().async_handshake(ssl::stream_base::client,
                                     beast::bind_front_handler(
                                             &session::on_ssl_handshake,
                                             shared_from_this()));
}


void session::on_ssl_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "ssl_handshake");

    beast::get_lowest_layer(ws_).expires_never();

    ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type &req)
            {
                req.set(http::field::user_agent,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        "websocket-ssl-client");
            }));

    ws_.async_handshake(host_, "/",
                        beast::bind_front_handler(
                                &session::on_handshake,
                                shared_from_this()));
}


void session::on_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "handshake");

    ws_.async_write(net::buffer(text_),
                    beast::bind_front_handler(
                            &session::on_write,
                            shared_from_this()));
}


void session::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
    ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    ws_.async_read(buffer_,
                   beast::bind_front_handler(
                           &session::on_read,
                           shared_from_this()));
}


void session::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "read");

    ws_.async_close(websocket::close_code::normal,

                    beast::bind_front_handler(
                            &session::on_close,
                            shared_from_this()));
}

void session::on_close(beast::error_code ec)
{
    if (ec)
        return fail(ec, "close");

    std::cout << beast::make_printable(buffer_.data()) << std::endl;
}
