#include "smtp_server.h"
#include <boost/asio.hpp>
#include <pqxx/transaction>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace maild;
using boost::asio::ip::tcp;

log4cxx::LoggerPtr smtp_server::logger(log4cxx::Logger::getLogger("smtp_server"));

smtp_server::smtp_server(boost::asio::io_service& io_service,  const server_options& options) 
    : options(options), acceptor(io_service,tcp::endpoint(tcp::v4(),options.get_plain_port())),
      db(options.get_db_connection_string())
{
  db.prepare("new_mail","insert into mails(from_address,to_address,body,date_received,username) values ($1,$2,$3,$4,$5)");
  log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("smtp_server"));
  LOG4CXX_INFO(logger, "Starting to accept connections on port "<<options.get_plain_port());
  start_accept();
}

smtp_server::~smtp_server()
{
  db.disconnect();
}

void smtp_server::start_accept()
{
    LOG4CXX_INFO(logger, "Waiting for client...");
    session_ptr new_session = std::make_shared<session>(acceptor.get_io_service(),options,[this](session* s){
        remove_session(s);
    });    
    acceptor.async_accept(new_session->get_socket(),[new_session,this](const boost::system::error_code& error){
        handle_accept(std::move(new_session),error);
    });
}

void smtp_server::handle_accept(session_ptr new_session, const boost::system::error_code& error)
{    
    if(!error)
    {    
        LOG4CXX_INFO(logger, "Got new client connection, starting session");        
        ao.send([this,new_session](){
           sessions.insert(new_session); 
           new_session->start();
           LOG4CXX_DEBUG(logger, "Sessions count "<<sessions.size());
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
            mail mail_message = (*it)->get_mail_message();
            sessions.erase(it);
            LOG4CXX_DEBUG(logger, "Sessions count "<<sessions.size());
            save_message(mail_message);
        }
    });
}

void smtp_server::save_message(const mail &mail_message)
{
  std::time_t now_time_t = std::time(nullptr);
  std::tm tm = *std::localtime(&now_time_t);
  std::stringstream time_str;
  time_str<<std::put_time(&tm, "%F %T %z");
  std::string username = mail_message.to.substr(0,mail_message.to.find('@'));
  pqxx::work w(db);
  w.prepared("new_mail")
      (mail_message.from)
      (mail_message.to)
      (mail_message.body)
      (time_str.str())
      (username)
      .exec();
  w.commit();
}
