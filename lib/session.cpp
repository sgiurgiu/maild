#include "session.h"
#include "utils.h"

#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <pqxx/connection>
#include <pqxx/transaction>

using namespace maild;

log4cxx::LoggerPtr session::logger(log4cxx::Logger::getLogger("session"));

session::session(boost::asio::io_context& io_context,
                 const server_options& options)
                : options(options),strand(boost::asio::make_strand(io_context)), socket(strand),
                  session_start(std::chrono::steady_clock::now())
{

}

session::~session()
{
    LOG4CXX_DEBUG(logger, "Deleting session");
}

boost::asio::ip::tcp::socket& session::get_socket()
{
    return socket;
}

mail session::get_mail_message() const
{
  return mail_message;
}
void session::start()
{
    on_start();
}
void session::on_start()
{
    using namespace std::placeholders;

    std::ostream request_stream(&request);
    std::string greeting_message = "220 "+options.get_domain_name()+" ESMTP MailD ready";
    request_stream << greeting_message << "\r\n";
    LOG4CXX_DEBUG(logger, "Starting session, writing "<<greeting_message<<", socket open:"<<socket.is_open());

    boost::asio::async_write(socket,request,std::bind(&session::handle_write_greeting,shared_from_this(),_1,_2));
}

void session::handle_write_greeting(const boost::system::error_code& error, size_t bytes_transferred)
{
    using namespace std::placeholders;
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error writing greeting "<<error.message());        
        return;
    }
    request.consume(bytes_transferred);
    boost::asio::async_read_until(socket,response,"\r\n",std::bind(&session::handle_read_greeting_response,shared_from_this(),_1,_2));
}
void session::handle_read_greeting_response(const boost::system::error_code& error, std::size_t bytes_transferred)
{    
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading greeting "<<error.message());
        return;
    }
    if(bytes_transferred <= 6) {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_read_greeting_response");
        return;
    }
    using namespace std::placeholders;
    std::string line(boost::asio::buffer_cast<const char*>(response.data()),bytes_transferred-2);
    std::string hello = line.substr(0,4);    
    std::string client_name = line.substr(5); 
    LOG4CXX_DEBUG(logger, "Got hello "<<hello<<", and client name: "<<client_name);
    response.consume(bytes_transferred);
    auto handle_read_commands_func = std::bind(&session::handle_read_commands,shared_from_this(),_1,_2);
    if(hello == "HELO")
    {
        std::ostream request_stream(&request);    
        request_stream << "250 Hello " << client_name << ", glad to meet you\r\n";    
        boost::asio::async_write(socket,request,handle_read_commands_func);
    }
    else if(hello == "EHLO")
    {
        std::ostream request_stream(&request);    
        request_stream << "250-"<< options.get_domain_name() <<" Hello " << client_name << "\r\n";    
        request_stream << "250-AUTH LOGIN\r\n";
        request_stream << "250 SIZE 1000000\r\n";    
        boost::asio::async_write(socket,request,handle_read_commands_func);
    } 
}

void session::handle_read_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading command "<<error.message());
        return;
    }
    using namespace std::placeholders;
    request.consume(bytes_transferred);
    boost::asio::async_read_until(socket,response,"\r\n",std::bind(&session::handle_write_commands,shared_from_this(),_1,_2));
}
void session::handle_write_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading command "<<error.message());
        return;
    }
    if(bytes_transferred <= 5) {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_write_commands");
        return;
    }
    
    std::string line(boost::asio::buffer_cast<const char*>(response.data()),bytes_transferred-2);
    std::string command = line.substr(0,4);
    LOG4CXX_DEBUG(logger, "Got command "<<command);       
    response.consume(bytes_transferred);
    if(command=="DATA")
    {
        handle_write_data_command(error,bytes_transferred);
    }
    else if (command == "QUIT")
    {
        handle_write_quit_command(error,bytes_transferred);
    }
    else
    {
        if(line.size() > 5)
        {
            std::string command_param = line.substr(5);
            LOG4CXX_DEBUG(logger, "Got command_param: "<<command_param);   
            if(command == "MAIL")
            {
                mail_message.from = utils::get_mail_from(command_param);
            } 
            else if(command == "RCPT")
            {
                mail_message.to = utils::get_mail_to(command_param);
            } 
            else if(command == "AUTH")
            {
                handle_auth_command(command_param);
                return;
            }
        }
        std::ostream request_stream(&request);    
        request_stream << "250 Ok\r\n";    
        using namespace std::placeholders;
        boost::asio::async_write(socket,request,std::bind(&session::handle_read_commands,shared_from_this(),_1,_2));
    }
}

void session::handle_auth_command(const std::string& command_param)
{
    if(command_param == "LOGIN")
    {
        std::ostream request_stream(&request);    
        request_stream << "334 VXNlcm5hbWU6\r\n";    
        boost::asio::async_write(socket,request,
                                [self = shared_from_this()](const boost::system::error_code& /*error*/, size_t bytes_transferred){
            self->request.consume(bytes_transferred);
            
            boost::asio::async_read_until(self->socket,self->response,"\r\n",
                                        [self = self->shared_from_this()](const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/){
                    
                    std::ostream request_stream(&self->request);
                    request_stream << "334 UGFzc3dvcmQ6\r\n";    
                    boost::asio::async_write(self->socket,self->request,
                                            [self = self->shared_from_this()](const boost::system::error_code& /*error*/, size_t bytes_transferred){
                        self->request.consume(bytes_transferred);
                        boost::asio::async_read_until(self->socket,self->response,"\r\n",
                                                    [self = self->shared_from_this()](const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/){
                                
                                std::ostream request_stream(&self->request);
                                request_stream << "235 2.7.0 Authentication successful\r\n";    
                                boost::asio::async_write(self->socket,self->request,
                                                        [self = self->shared_from_this()](const boost::system::error_code& error, size_t bytes_transferred){
                                        self->handle_read_commands(error,bytes_transferred);
                                });
                            });            
                    });
                });                
        });        
    }
}


void session::handle_write_data_command(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading data command "<<error.message());
        return;
    }
    response.consume(bytes_transferred);
    std::ostream request_stream(&request);    
    request_stream << "354 End data with <CR><LF>.<CR><LF>\r\n";
    using namespace std::placeholders;
    boost::asio::async_write(socket,request,std::bind(&session::handle_read_data_command,shared_from_this(),_1,_2));
}
void session::handle_read_data_command(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading data command "<<error.message());
        return;
    }
    request.consume(bytes_transferred);
    using namespace std::placeholders;
    boost::asio::async_read_until(socket,response,"\r\n.\r\n",std::bind(&session::handle_write_data_response,shared_from_this(),_1,_2));
}
void session::handle_write_data_response(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading data contents "<<error.message());
        return;
    }
    if(bytes_transferred <= 6) 
    {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_write_data_response");
        return;
    }
    
    std::string data(boost::asio::buffer_cast<const char*>(response.data()),bytes_transferred-5);
    LOG4CXX_DEBUG(logger, "Got data "<<data);       
    mail_message.body = data;
    response.consume(bytes_transferred);
    std::ostream request_stream(&request);    
    request_stream << "250 Ok\r\n";
    using namespace std::placeholders;
    boost::asio::async_write(socket,request,std::bind(&session::handle_read_commands,shared_from_this(),_1,_2));
}

void session::handle_write_quit_command(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading quit command "<<error.message());
        return;
    }
    response.consume(bytes_transferred);
    std::ostream request_stream(&request);    
    request_stream << "221 Bye\r\n";    
    boost::asio::async_write(socket,request,
                             [self = shared_from_this()](const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/){
        //save message
        if(self->mail_message.from.empty() || self->mail_message.to.empty() || self->mail_message.body.empty()) {
            return;
        }
        pqxx::connection db(self->options.get_db_connection_string());
        db.prepare("new_mail","insert into mails(from_address,to_address,body,date_received,username) values ($1,$2,$3,NOW(),$4)");
        std::string username = self->mail_message.to.substr(0,self->mail_message.to.find('@'));
        pqxx::work w(db);
        w.exec_prepared("new_mail",self->mail_message.from,self->mail_message.to,self->mail_message.body,
            username);
        w.commit();
    });

}
