#ifndef MAILD_SESSION_H
#define MAILD_SESSION_H

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include <string>
#include <log4cxx/logger.h>

namespace maild {
class smtp_server;
class session
{
public:
    session(boost::asio::io_service& io_service,smtp_server* server);
    ~session() ;//= default;
    session ( const session& ) = delete;
    session ( const session&& ) = delete;
    session& operator= ( const session& ) = delete;
    session& operator= ( const session&& ) = delete;

    boost::asio::ip::tcp::socket& get_socket();
    void start();
private:    
    void handle_write_greeting(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_read_greeting_response(const boost::system::error_code& error,
                                       std::size_t bytes_transferred);
    void handle_read_commands(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write_commands(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write_data_command(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_read_data_command(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write_data_response(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write_quit_command(const boost::system::error_code& error, std::size_t bytes_transferred);
private:
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf response;
    boost::asio::streambuf request; 
    smtp_server* server;
    static log4cxx::LoggerPtr logger;
};

typedef std::shared_ptr<session> session_ptr;
}

#endif // MAILD_SESSION_H
