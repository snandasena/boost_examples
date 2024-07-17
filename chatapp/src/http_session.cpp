//
// Created by sajith.nandasena on 11.07.2024.
//
#include "http_session.h"
#include "websocket_session.h"
#include <boost/config.hpp>
#include <iostream>

beast::string_view mime_type(beast::string_view path);

std::string path_cat(beast::string_view base, beast::string_view path);

template<class Body, class Allocator>
http::message_generator handle_request(beast::string_view doc_root,
                                       http::request<Body, http::basic_fields<Allocator> > &&req)
{
    auto const bad_request = [&req](beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    auto const not_found = [&req](beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error = [&req](beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occured: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };


    if (req.method() != http::verb::get && req.method() != http::verb::head)
        return bad_request("Unknown HTTP-method");

    if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos)
        return bad_request("Illegal request-target");

    std::string path = path_cat(doc_root, req.target());

    std::cout << "PATH : " << path << std::endl;
    if (req.target().back() == '/')
        path.append("index.html");

    beast::error_code ec;
    http::file_body::value_type body;
    body.open("index.html", beast::file_mode::scan, ec);

    if (ec == boost::system::errc::no_such_file_or_directory)
        return not_found(req.target());

    if (ec)
        return server_error(ec.message());

    const auto size = body.size();
    if (req.method() == http::verb::head)
    {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    http::response<http::file_body> res{
        std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(http::status::ok, req.version())
    };

    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}


beast::string_view mime_type(beast::string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
    {
        const auto pos = path.find(".");
        if (pos == beast::string_view::npos)
            return beast::string_view{};

        return path.substr(pos);
    }();

    if (iequals(ext, ".htm")) return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php")) return "text/html";
    if (iequals(ext, ".css")) return "text/css";
    if (iequals(ext, ".txt")) return "text/plain";
    if (iequals(ext, ".js")) return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml")) return "application/xml";
    if (iequals(ext, ".swf")) return "application/x-shockwave-flash";
    if (iequals(ext, ".flv")) return "video/x-flv";
    if (iequals(ext, ".png")) return "image/png";
    if (iequals(ext, ".jpe")) return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg")) return "image/jpeg";
    if (iequals(ext, ".gif")) return "image/gif";
    if (iequals(ext, ".bmp")) return "image/bmp";
    if (iequals(ext, ".ico")) return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif")) return "image/tiff";
    if (iequals(ext, ".svg")) return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

std::string path_cat(beast::string_view base, beast::string_view path)
{
    if (base.empty())
        return std::string{path};

    std::string result{base};

#ifdef BOOST_MSVC
    char constexpr path_separator  = '\\';
    if(result.back() == path_separator )
        result.resize(result.size() -1);
    result.append(path.data(), path.size());

    for(auto &c: result)
        if(c == '/')
            c = path_separator;

#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());

#endif
    return result;
}


http_session::http_session(tcp::socket &&socket,
                           boost::shared_ptr<shared_state> const &state): stream_(std::move(socket)),
                                                                          state_(state)
{
}

void http_session::run()
{
    do_read();
}

void http_session::fail(beast::error_code ec, char const *what)
{
    if (ec == net::error::operation_aborted)
        return;

    std::cerr << what << ": " << ec.message() << std::endl;
}


void http_session::do_read()
{
    parser_.emplace();
    parser_->body_limit(10000);

    stream_.expires_after(std::chrono::seconds(30));

    http::async_read(stream_, buffer_, parser_->get(),
                     beast::bind_front_handler(&http_session::on_read, shared_from_this()));
}

void http_session::on_read(beast::error_code ec, std::size_t)
{
    if (ec == http::error::end_of_stream)
    {
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    if (ec)
        return fail(ec, "read");

    if (websocket::is_upgrade(parser_->get()))
    {
        boost::make_shared<websocket_session>(stream_.release_socket(), state_)->run(parser_->release());
        return;
    }

    http::message_generator msg = handle_request(state_->doc_root(), parser_->release());

    bool keep_live = msg.keep_alive();
    auto self = shared_from_this();

    beast::async_write(stream_, std::move(msg), [self, keep_live](beast::error_code ec, std::size_t bytes)
    {
        self->on_write(ec, bytes, keep_live);
    });
}

void http_session::on_write(beast::error_code ec, std::size_t, bool keep_alive)
{
    if (ec)
        return fail(ec, "write");

    if (!keep_alive)
    {
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
        return;
    }
    do_read();
}
