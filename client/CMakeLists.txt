
set(SIMPLE_WEBSOCKET_CLIENT SimpleWebSocketClient)

add_executable(${SIMPLE_WEBSOCKET_CLIENT} beast_websocket_client.cpp)

# Include Boost headers
target_include_directories(${SIMPLE_WEBSOCKET_CLIENT} PRIVATE ${Boost_INCLUDE_DIRS})

# Link Boost libraries
target_link_libraries(${SIMPLE_WEBSOCKET_CLIENT} PRIVATE ${Boost_LIBRARIES})
