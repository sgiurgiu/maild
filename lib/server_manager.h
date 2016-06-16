#ifndef MAILD_SERVER_MANAGER_H
#define MAILD_SERVER_MANAGER_H

#include "server_options.h"
#include <boost/asio/io_service.hpp>

namespace maild {

class server_manager
{
public:
    server_manager(const server_options& options);
    server_manager();
    void set_options(const server_options& options);
    void run();
    void stop();
private:
    server_options options;    
    boost::asio::io_service io_service;
};

}

#endif // MAILD_SERVER_MANAGER_H
