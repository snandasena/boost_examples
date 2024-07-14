//
// Created by sajith.nandasena on 10.07.2024.
//

#ifndef NET_H
#define NET_H

#include <boost/asio.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

#endif //NET_H
