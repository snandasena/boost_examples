
set(SIMPLE_WEBSOCKET_SERVER SimpleWebSocketServer)

add_executable(${SIMPLE_WEBSOCKET_SERVER} beast_websocket_server.cpp)

# Include Boost headers
target_include_directories(${SIMPLE_WEBSOCKET_SERVER} PRIVATE ${Boost_INCLUDE_DIRS})

# Link Boost libraries
target_link_libraries(${SIMPLE_WEBSOCKET_SERVER} PRIVATE ${Boost_LIBRARIES})
