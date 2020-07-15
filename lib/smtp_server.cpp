#include "smtp_server.h"
#include <boost/asio.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <boost/bind.hpp>

using namespace maild;
using boost::asio::ip::tcp;

log4cxx::LoggerPtr smtp_server::logger(log4cxx::Logger::getLogger("smtp_server"));

smtp_server::smtp_server(boost::asio::io_service& io_service, const std::string& listen_address, const server_options& options)
    : options(options),io_service(io_service),acceptor(io_service)
{
    tcp::endpoint endpoint(boost::asio::ip::address::from_string(listen_address),
                           options.get_plain_port());
   acceptor.open(endpoint.protocol());
   acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
   acceptor.bind(endpoint);
   acceptor.listen();
   LOG4CXX_INFO(logger, "Starting to accept connections on address "<<listen_address<<" and port "<<options.get_plain_port());
}

smtp_server::~smtp_server()
{
}

void smtp_server::run()
{
    start_accept();
}
void smtp_server::start_accept()
{
    LOG4CXX_INFO(logger, "Waiting for client...");    
    session_ptr new_session = std::make_shared<session>(io_service,options);
    acceptor.async_accept(new_session->get_socket(),
                          boost::bind(&smtp_server::handle_accept,this,boost::asio::placeholders::error,
                                      new_session));
}

void smtp_server::handle_accept(const boost::system::error_code& error,session_ptr new_session)
{    
    if(!error)
    {    
        LOG4CXX_INFO(logger, "Got new client connection, starting session");
        new_session->start();

        start_accept();
    }
    else
    {
        LOG4CXX_ERROR(logger, "Could not accept new connection from client. Message:"<<error.message());
    }
}


