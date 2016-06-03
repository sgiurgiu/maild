#ifndef MAILD_SERVER_H
#define MAILD_SERVER_H

#include "mail.h"
#include "session.h"
#include "active_object.h"
#include "server_options.h"
#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <log4cxx/logger.h>
#include <unordered_set>
#include <functional>
#include <pqxx/connection>

namespace maild {

class smtp_server
{
public:
    smtp_server(boost::asio::io_service& io_service, const server_options& options);    
    ~smtp_server();
    smtp_server ( const smtp_server& ) = delete;
    smtp_server (smtp_server&& ) = delete;
    smtp_server& operator= ( const smtp_server& ) = delete;
    smtp_server& operator= (smtp_server&& ) = delete;
        
private:
    void remove_session(session* s);
    void start_accept();
    void handle_accept(session_ptr new_session, const boost::system::error_code& error);
    void save_message(const mail& mail_message);
private:
    template<class T> struct session_ptr_equals
    {
        constexpr bool operator()( const T& lhs, const T& rhs ) const
        {            
            return lhs.get()==rhs.get();
        }
    };    
    server_options options;  
    boost::asio::ip::tcp::acceptor acceptor;    
    active_object ao;
    std::unordered_set<session_ptr,std::hash<session_ptr>,session_ptr_equals<session_ptr>> sessions;
    pqxx::connection db;
    static log4cxx::LoggerPtr logger;
};
}

#endif // MAILD_SERVER_H
