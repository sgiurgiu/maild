#include "smtp_server.h"

#include <boost/asio.hpp>

using namespace maild;
using boost::asio::ip::tcp;

log4cxx::LoggerPtr smtp_server::logger(log4cxx::Logger::getLogger("smtp_server"));

smtp_server::smtp_server(boost::asio::io_service& io_service,  unsigned short port) 
    : acceptor(io_service,tcp::endpoint(tcp::v4(),port))
{
    log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("smtp_server"));
    LOG4CXX_INFO(logger, "Starting to accept connections on port "<<port);
    start_accept();
}

void smtp_server::start_accept()
{
    session_ptr new_session = std::make_shared<session>(acceptor.get_io_service(),this);    
    acceptor.async_accept(new_session->get_socket(),[new_session,this](const boost::system::error_code& error){
        handle_accept(std::move(new_session),error);
    });
}

void smtp_server::handle_accept(session_ptr new_session, const boost::system::error_code& error)
{    
    if(!error)
    {    
        LOG4CXX_INFO(logger, "Got new client connection, starting session");
        new_session->start();
        ao.send([this,new_session](){
           sessions.insert(new_session); 
        });        
        start_accept();
    }
    else
    {
        LOG4CXX_ERROR(logger, "Could not accept new connection from client. Message:"<<error.message());
    }
}

void smtp_server::remove_session(session* s)
{
    ao.send([this,s](){
        
        std::shared_ptr<session> dummy_ptr(s, [](session*){});//won't delete
        auto it = sessions.find(dummy_ptr);
        if(it != sessions.end())
        {
            sessions.erase(it);
        }
    });
}
