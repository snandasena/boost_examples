//
// Created by sajith.nandasena on 10.07.2024.
//

#include "websocket_session.h"
#include "shared_state.h"

#include <iostream>

websocket_session::websocket_session(tcp::socket socket,
                                     boost::shared_ptr<shared_state> const &state) : ws_(std::move(socket)),
                                                                                     state_(state)
{
}

websocket_session::~websocket_session()
{
    state_->leave(this);
}

void websocket_session::fail(beast::error_code ec, char const *what)
{
    if (ec == net::error::operation_aborted || ec == websocket::error::closed)
        return;

    std::cerr << what << " : " << ec.message() << std::endl;
}


void websocket_session::on_accept(beast::error_code ec)
{
    if (ec)
        return fail(ec, "accept");

    state_->join(this);

    ws_.async_read(buffer_, beast::bind_front_handler(&websocket_session::on_read, shared_from_this()));
}

void websocket_session::on_read(beast::error_code ec, std::size_t)
{
    if (ec)
        return fail(ec, "read");

    std::cout<<beast::make_printable(buffer_.data())<<std::endl;
    // send to all connections
    state_->send(beast::buffers_to_string(buffer_.data()));
    // clear the buffer
    buffer_.consume(buffer_.size());

    ws_.async_read(buffer_, beast::bind_front_handler(&websocket_session::on_read, shared_from_this()));
}


void websocket_session::send(boost::shared_ptr<std::string const> const &ss)
{
    net::post(ws_.get_executor(),
              beast::bind_front_handler(&websocket_session::on_send, shared_from_this(), ss));
}

void websocket_session::on_send(boost::shared_ptr<std::string const> const &ss)
{
    queue_.push_back(ss);

    if (queue_.size() > 1)
        return;

    ws_.async_write(net::buffer(*queue_.front()),
                    beast::bind_front_handler(&websocket_session::on_write, shared_from_this()));
}

void websocket_session::on_write(beast::error_code ec, std::size_t)
{
    if (ec)
        return fail(ec, "write");

    queue_.erase(queue_.begin());

    if (!queue_.empty())
        ws_.async_write(net::buffer(*queue_.front()),
                        beast::bind_front_handler(&websocket_session::on_write, shared_from_this()));
}
