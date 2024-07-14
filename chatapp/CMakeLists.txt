
project(ChatApp)


find_package(Boost REQUIRED COMPONENTS system)
find_package(Asio REQUIRED)


file(GLOB APP_FILES
        src/main.cpp
        src/beast.h
        src/net.h
        src/shared_state.h
        src/shared_state.cpp
        src/websocket_session.h
        src/websocket_session.cpp
        src/http_session.h
        src/http_session.cpp
        src/listener.h
        src/listener.cpp
)

add_executable(${PROJECT_NAME} ${APP_FILES})

target_link_libraries(${PROJECT_NAME} Boost::system asio::asio)