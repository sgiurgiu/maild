#ifndef MAILD_SESSION_H
#define MAILD_SESSION_H

#include "server_options.h"
#include "mail.h"
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include <string>
#include <functional>

#include <log4cxx/logger.h>

namespace maild {
class session
{
public:
    typedef std::function<void(session*)> complete_message_handler;
    session(boost::asio::io_service& io_service,const server_options& options,complete_message_handler message_handler);
    ~session() ;//= default;
    session ( const session& ) = delete;
    session ( const session&& ) = delete;
    session& operator= ( const session& ) = delete;
    session& operator= ( const session&& ) = delete;

    boost::asio::ip::tcp::socket& get_socket();
    void start();
    mail get_mail_message() const;
    
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
    server_options options;  
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf response;
    boost::asio::streambuf request; 
    complete_message_handler message_handler;
    mail mail_message;
    static log4cxx::LoggerPtr logger;
};

typedef std::shared_ptr<session> session_ptr;
}

#endif // MAILD_SESSION_H
