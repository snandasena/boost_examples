project(ssl_server)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Asio REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB SSL_SERVER_FILE
        src/common.h
        src/ignore_unused.h
        src/session.h
        src/session.cpp
        src/listener.h
        src/listener.cpp
        src/ssl_websocket_server.cpp
)

add_executable(${PROJECT_NAME} ${SSL_SERVER_FILE})

target_link_libraries(${PROJECT_NAME} PUBLIC commonutils Boost::system asio::asio OpenSSL::SSL OpenSSL::Crypto)