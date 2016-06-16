#include "server_manager.h"
#include "smtp_server.h"



using namespace maild;
using boost::asio::ip::tcp;

server_manager::server_manager(const server_options& options):options(options)
{

}
server_manager::server_manager() {}
void server_manager::set_options(const server_options &options)
{
  this->options = options;
}

void server_manager::run()
{
    smtp_server server(io_service,options);    
    io_service.run();    
}

void server_manager::stop()
{
  io_service.stop();
}
