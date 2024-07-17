project(commonutils)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Asio REQUIRED)
find_package(OpenSSL REQUIRED)


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} INTERFACE Boost::system asio::asio OpenSSL::SSL OpenSSL::Crypto)