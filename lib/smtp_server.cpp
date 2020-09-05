#include "smtp_server.h"
#include <boost/asio.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <boost/bind.hpp>
#include <spdlog/spdlog.h>

using namespace maild;
using boost::asio::ip::tcp;

smtp_server::smtp_server(boost::asio::io_service& io_service, pqxx::connection *db,
                         const server& server_options,const std::string& domain_name,
                         const certificates& certificate_files)
    : db(db),domain_name(domain_name),certificate_files(certificate_files),
      io_service(io_service),acceptor(io_service)
{
    tcp::endpoint endpoint(boost::asio::ip::address::from_string(server_options.ip),
                           server_options.port);
   acceptor.open(endpoint.protocol());
   acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
   acceptor.bind(endpoint);
   acceptor.listen();
   spdlog::info("Starting to accept connections on address {} and port {}",server_options.ip,server_options.port);
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
    spdlog::info( "Waiting for client...");
    session_ptr new_session = std::make_shared<session>(io_service,db,domain_name,certificate_files);
    acceptor.async_accept(new_session->get_socket(),
                          boost::bind(&smtp_server::handle_accept,this,boost::asio::placeholders::error,
                                      new_session));
}

void smtp_server::handle_accept(const boost::system::error_code& error,session_ptr new_session)
{    
    if(!error)
    {    
        spdlog::info( "Got new client connection, starting session");
        new_session->start();

        start_accept();
    }
    else
    {
        spdlog::error( "Could not accept new connection from client. Message: {}",error.message());
    }
}


