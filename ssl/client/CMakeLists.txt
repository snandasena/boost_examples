project(ssl_client)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Asio REQUIRED)
find_package(OpenSSL REQUIRED)


add_executable(${PROJECT_NAME}
        src/session.h
        src/session.cpp
        src/ssl_websocket_client.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC commonutils Boost::system asio::asio OpenSSL::SSL OpenSSL::Crypto)