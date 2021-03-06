#ifndef MAILD_SESSION_H
#define MAILD_SESSION_H

#include "server_options.h"
#include "mail.h"
#include "smtp_command.h"

#include "maild_socket.h"

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/steady_timer.hpp>
#include <memory>
#include <string>
#include <chrono>
#include <map>
#include <pqxx/connection>

namespace maild {
class session : public std::enable_shared_from_this<session>
{
public:
    session(const boost::asio::any_io_executor& executor, const std::string& db_connection_string,
            const std::string& domain_name, const certificates& certificate_files,
            bool is_fully_ssl);
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
    std::shared_ptr<session> get_session()
    {
        return shared_from_this();
    }

private:    
    void on_start();
    void handle_write_greeting(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_read_greeting_response(const boost::system::error_code& error,
                                       std::size_t bytes_transferred);
    void handle_read_commands(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_parse_commands(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write_data_command(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_read_data_command(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_write_data_response(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_complete_quit_command(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_auth_command(const std::string& command_param);
    void close_socket_timeout(const boost::system::error_code& error);
private:
    std::string db_connection_string;
    std::string domain_name;
    boost::asio::strand<boost::asio::any_io_executor> strand;
    maild_socket socket;
    boost::asio::streambuf response;
    boost::asio::streambuf request; 
    mail mail_message;
    std::chrono::time_point<std::chrono::steady_clock> session_start;
    std::map<std::string,std::unique_ptr<smtp_command>> commands;
    bool is_fully_ssl;
    boost::asio::steady_timer timer;
};

typedef std::shared_ptr<session> session_ptr;
}

#endif // MAILD_SESSION_H
