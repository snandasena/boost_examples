//
// Created by sajith.nandasena on 10.07.2024.
//

#ifndef WEB_SOCKET_SESSION_H
#define WEB_SOCKET_SESSION_H


#include "net.h"
#include "beast.h"

#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

class shared_state;

class websocket_session : public boost::enable_shared_from_this<websocket_session>
{
    beast::flat_buffer buffer_;
    websocket::stream<tcp::socket> ws_;
    boost::shared_ptr<shared_state> state_;
    std::vector<boost::shared_ptr<std::string const> > queue_;

    void fail(beast::error_code ec, char const *what);

    void on_accept(beast::error_code ec);

    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    void on_write(beast::error_code ec, std::size_t bytes_transferred);

public:
    websocket_session(tcp::socket socket, boost::shared_ptr<shared_state> const &state);

    ~websocket_session();

    template<class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator> > req);

    void send(boost::shared_ptr<std::string const> const &ss);

private:
    void on_send(boost::shared_ptr<std::string const> const &ss);
};

template<class Body, class Allocator>
void websocket_session::run(http::request<Body, http::basic_fields<Allocator> > req)
{
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type &res)
    {
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + "websocket-chat-multi");
    }));

    ws_.async_accept(req, beast::bind_front_handler(&websocket_session::on_accept, shared_from_this()));
}

#endif //WEB_SOCKET_SESSION_H
