project(coroutine_websocket_server)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Asio REQUIRED)

file(GLOB SERVER_FILE
        src/awaitable_websocket_server.cpp
)

add_executable(${PROJECT_NAME} ${SERVER_FILE})

target_link_libraries(${PROJECT_NAME} PUBLIC commonutils Boost::system asio::asio)
