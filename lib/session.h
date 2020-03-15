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
#include <chrono>
#include <log4cxx/logger.h>

namespace maild {
class session
{
public:
    typedef std::function<void(session*)> complete_message_handler;
#if BOOST_VERSION_NUMBER_PATCH(BOOST_VERSION) >= 7000
    session(const boost::asio::executor& executor, const server_options& options, complete_message_handler quit_handler);
#else
    session(boost::asio::io_service& io_service, const server_options& options, complete_message_handler quit_handler);
#endif
    ~session() ;//= default;
    session ( const session& ) = delete;
    session ( session&& ) = delete;
    session& operator= ( const session& ) = delete;
    session& operator= ( session&& ) = delete;

    boost::asio::ip::tcp::socket& get_socket();
    void start();
    mail get_mail_message() const;
    std::chrono::time_point<std::chrono::steady_clock> get_session_start_time() const
    {
        return session_start;
    }
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
    void handle_auth_command(const std::string& command_param);
private:
    server_options options;  
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf response;
    boost::asio::streambuf request; 
    complete_message_handler quit_handler;
    mail mail_message;
    std::chrono::time_point<std::chrono::steady_clock> session_start;
    static log4cxx::LoggerPtr logger;
};

typedef std::shared_ptr<session> session_ptr;
}

#endif // MAILD_SESSION_H
