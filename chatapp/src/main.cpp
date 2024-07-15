//
// Created by sajith.nandasena on 10.07.2024.
//

#include "listener.h"
#include "shared_state.h"

#include <boost/asio/signal_set.hpp>
#include <boost/smart_ptr.hpp>
#include <iostream>
#include <vector>


int main()
{

    net::io_context ioc;
    const auto adress = net::ip::make_address("0.0.0.0");
    const auto port = 3000u;
    const auto doc_root = "/"; 
    const auto threads = 1;

    net::io_context ioc;

    boost::make_shared<listener>(ioc, tcp::endpoint{address, port}, boost::make_shared<shared_state>(doc_root))->run();


}