#ifndef MAILD_SERVER_H
#define MAILD_SERVER_H

#include "session.h"
#include "active_object.h"
#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>
#include <log4cxx/logger.h>
#include <unordered_set>
#include <functional>
namespace maild {

class smtp_server
{
public:
    smtp_server(boost::asio::io_service& io_service, unsigned short port);    
    ~smtp_server() = default;
    smtp_server ( const smtp_server& ) = delete;
    smtp_server (smtp_server&& ) = delete;
    smtp_server& operator= ( const smtp_server& ) = delete;
    smtp_server& operator= (smtp_server&& ) = delete;
    
    void remove_session(session* s);
private:
    void start_accept();
    void handle_accept(session_ptr new_session, const boost::system::error_code& error);
private:
    template<class T> struct session_ptr_equals
    {
        constexpr bool operator()( const T& lhs, const T& rhs ) const
        {            
            return lhs.get()==rhs.get();
        }
    };    
    boost::asio::ip::tcp::acceptor acceptor;
    std::unordered_set<session_ptr,std::hash<session_ptr>,session_ptr_equals<session_ptr>> sessions;
    active_object ao;
    static log4cxx::LoggerPtr logger;
};
}

#endif // MAILD_SERVER_H
