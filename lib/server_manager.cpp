#include "server_manager.h"
#include "smtp_server.h"

#include <boost/asio.hpp>

using namespace maild;
using boost::asio::ip::tcp;

server_manager::server_manager(const server_options& options):options(options)
{

}

void server_manager::run()
{
    boost::asio::io_service io_service;
    smtp_server server(io_service,options.get_plain_port());    
    io_service.run();
}
