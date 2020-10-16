#ifndef MAILD_SERVER_MANAGER_H
#define MAILD_SERVER_MANAGER_H

#include "server_options.h"
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>

namespace maild {

class server_manager
{
public:
    server_manager(const server_options& options);
    server_manager();
    ~server_manager();
    void run();
    void stop();
private:
    void cleanup_messages(const boost::system::error_code& error);
    void prepare_database();
private:
    server_options options;    
    boost::asio::io_context io_context;
    boost::asio::io_context cleanup_context;
    boost::asio::signal_set signals;
    boost::asio::steady_timer cleanup_timer;
};

}

#endif // MAILD_SERVER_MANAGER_H
