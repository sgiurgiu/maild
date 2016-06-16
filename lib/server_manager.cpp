#include "server_manager.h"
#include "smtp_server.h"
#include <vector>
#include <memory>


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
  std::vector<std::unique_ptr<smtp_server>> servers;
  for(const auto& address : options.get_ips())
  {
    servers.push_back(std::make_unique<smtp_server>(io_service,address,options))  ;
  }
  io_service.run();
}

void server_manager::stop()
{
  io_service.stop();
}
