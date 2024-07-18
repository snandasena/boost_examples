//
// Created by sajith.nandasena on 16.07.2024.
//

#include "session.h"

#include <utils/common.h>
#include <certs/root_certificate.h>

int main()
{
    const auto host = "0.0.0.0";
    const auto port = "3000";
    const auto text = "Hello from the ssl client";

    net::io_context ioc{};

    ssl::context cxt{ssl::context::tlsv12_client};
    load_root_certificates(cxt);

    std::make_shared<session>(ioc, cxt)->run(host, port, text);
    ioc.run();

    return 0;
}
