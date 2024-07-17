//
// Created by sajith.nandasena on 16.07.2024.
//

#include "session.h"

session::session(tcp::socket &&socket, ssl::context &ctx): ws_(std::move(socket), ctx)
{
}

void session::run()
{
    net::dispatch(ws_.get_executor(), beast::bind_front_handler(&session::on_run, shared_from_this()));
}

void session::on_run()
{
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds{30});

    ws_.next_layer().async_handshake(ssl::stream_base::server,
                                     beast::bind_front_handler(&session::on_handshake, shared_from_this()));
}

void session::on_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "handshake");

    beast::get_lowest_layer(ws_).expires_never();

    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type &res)
    {
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-async-server");
    }));

    ws_.async_accept(beast::bind_front_handler(&session::on_accept, shared_from_this()));
}

void session::on_accept(beast::error_code ec)
{
    if (ec)
        fail(ec, "accept");

    do_read();
}


void session::do_read()
{
    ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    ignore_unused(bytes_transferred);

    if (ec == websocket::error::closed)
        return;

    if (ec)
        fail(ec, "read");

    ws_.text(ws_.got_text());
    ws_.async_write(buffer_.data(), beast::bind_front_handler(&session::on_write, shared_from_this()));
}


void session::on_write(beast::error_code ec, std::size_t byte_transferred)
{
    ignore_unused(byte_transferred);

    if (ec)
        return fail(ec, "write");

    buffer_.consume(buffer_.size());
    do_read();
}
