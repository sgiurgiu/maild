#ifndef MAILD_SERVER_H
#define MAILD_SERVER_H

#include "mail.h"
#include "session.h"
#include "server_options.h"
#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <unordered_set>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <pqxx/connection>

namespace maild {

class smtp_server
{
public:
    smtp_server(const boost::asio::any_io_executor& executor,
                const std::string& db_connection_string,
                const server& server_options,
                const std::string& domain_name,
                const certificates& certificate_files);
    ~smtp_server();
    smtp_server ( const smtp_server& ) = delete;
    smtp_server (smtp_server&& ) = delete;
    smtp_server& operator= ( const smtp_server& ) = delete;
    smtp_server& operator= (smtp_server&& ) = delete;
    void run();
private:
    void start_accept();
    void handle_accept(const boost::system::error_code& error,session_ptr new_session);
    void save_message(const mail& mail_message);
private:
    template<class T> struct session_ptr_equals
    {
        constexpr bool operator()( const T& lhs, const T& rhs ) const
        {            
            return lhs.get()==rhs.get();
        }
    };
    std::string db_connection_string;
    std::string domain_name;
    certificates certificate_files;
    boost::asio::any_io_executor executor;
    boost::asio::ip::tcp::acceptor acceptor;
    server server_options;
};
}

#endif // MAILD_SERVER_H
