#include "session.h"
#include "utils.h"

#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>

using namespace maild;

log4cxx::LoggerPtr session::logger(log4cxx::Logger::getLogger("session"));

session::session(const boost::asio::executor& executor,
                 const server_options& options, complete_message_handler quit_handler)
                : options(options), socket(executor),quit_handler(quit_handler),
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
    std::ostream request_stream(&request);
    std::string greeting_message = "220 "+options.get_domain_name()+" ESMTP MailD ready";
    request_stream << greeting_message << "\r\n";
    LOG4CXX_DEBUG(logger, "Starting session, writing "<<greeting_message);
    boost::asio::async_write(socket,request,
                             [this](const boost::system::error_code& error, size_t bytes_transferred){
        handle_write_greeting(error,bytes_transferred);
      });
}

void session::handle_write_greeting(const boost::system::error_code& error, size_t bytes_transferred)
{    
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error writing greeting "<<error.message());
        quit_handler(this);
        return;
    }
    request.consume(bytes_transferred);
    boost::asio::async_read_until(socket,response,"\r\n",
                            [this](const boost::system::error_code& error, std::size_t bytes_transferred){
        handle_read_greeting_response(error,bytes_transferred);                                
    });
}
void session::handle_read_greeting_response(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading greeting "<<error.message());
        quit_handler(this);
        return;
    }
    if(bytes_transferred <= 6) {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_read_greeting_response");
        quit_handler(this);
        return;
    }
    std::string line(boost::asio::buffer_cast<const char*>(response.data()),bytes_transferred-2);
    std::string hello = line.substr(0,4);    
    std::string client_name = line.substr(5); 
    LOG4CXX_DEBUG(logger, "Got hello "<<hello<<", and client name: "<<client_name);
    response.consume(bytes_transferred);
    if(hello == "HELO")
    {
        std::ostream request_stream(&request);    
        request_stream << "250 Hello " << client_name << ", glad to meet you\r\n";    
        boost::asio::async_write(socket,request,
                                [this](const boost::system::error_code& error, size_t bytes_transferred){
            handle_read_commands(error,bytes_transferred);
        });        
    }
    else if(hello == "EHLO")
    {
        std::ostream request_stream(&request);    
        request_stream << "250-"<< options.get_domain_name() <<" Hello " << client_name << "\r\n";    
        request_stream << "250-AUTH LOGIN\r\n";
        request_stream << "250 SIZE 1000000\r\n";    
        boost::asio::async_write(socket,request,
                                [this](const boost::system::error_code& error, size_t bytes_transferred){
            handle_read_commands(error,bytes_transferred);
        });                
    } 
    else 
    {
        quit_handler(this);
    }
}

void session::handle_read_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading command "<<error.message());
        quit_handler(this);
        return;
    }

    request.consume(bytes_transferred);
    boost::asio::async_read_until(socket,response,"\r\n",
                            [this](const boost::system::error_code& error, std::size_t bytes_transferred){
        handle_write_commands(error,bytes_transferred);
    });    
}
void session::handle_write_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading command "<<error.message());
        quit_handler(this);
        return;
    }
    if(bytes_transferred <= 5) {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_write_commands");
        quit_handler(this);
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
        boost::asio::async_write(socket,request,
                                [this](const boost::system::error_code& error, size_t bytes_transferred){
            handle_read_commands(error,bytes_transferred);
        });
    }
}

void session::handle_auth_command(const std::string& command_param)
{
    if(command_param == "LOGIN")
    {
        std::ostream request_stream(&request);    
        request_stream << "334 VXNlcm5hbWU6\r\n";    
        boost::asio::async_write(socket,request,
                                [this](const boost::system::error_code& /*error*/, size_t bytes_transferred){
            request.consume(bytes_transferred);
            
            boost::asio::async_read_until(socket,response,"\r\n",
                                        [this](const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/){
                    
                    std::ostream request_stream(&request);    
                    request_stream << "334 UGFzc3dvcmQ6\r\n";    
                    boost::asio::async_write(socket,request,
                                            [this](const boost::system::error_code& /*error*/, size_t bytes_transferred){
                        request.consume(bytes_transferred);
                        boost::asio::async_read_until(socket,response,"\r\n",
                                                    [this](const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/){
                                
                                std::ostream request_stream(&request);    
                                request_stream << "235 2.7.0 Authentication successful\r\n";    
                                boost::asio::async_write(socket,request,
                                                        [this](const boost::system::error_code& error, size_t bytes_transferred){
                                        handle_read_commands(error,bytes_transferred);
                                });
                            });            
                    });
                });                
        });        
    }
    else
    {
        quit_handler(this);
    }
}


void session::handle_write_data_command(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading data command "<<error.message());
        quit_handler(this);
        return;
    }
    response.consume(bytes_transferred);
    std::ostream request_stream(&request);    
    request_stream << "354 End data with <CR><LF>.<CR><LF>\r\n";    
    boost::asio::async_write(socket,request,
                             [this](const boost::system::error_code& error, size_t bytes_transferred){
        handle_read_data_command(error,bytes_transferred);
    });
}
void session::handle_read_data_command(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading data command "<<error.message());
        quit_handler(this);
        return;
    }
    request.consume(bytes_transferred);
    boost::asio::async_read_until(socket,response,"\r\n.\r\n",
                            [this](const boost::system::error_code& error, std::size_t bytes_transferred){
        handle_write_data_response(error,bytes_transferred);
    });    
}
void session::handle_write_data_response(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading data contents "<<error.message());
        quit_handler(this);
        return;
    }
    if(bytes_transferred <= 6) 
    {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_write_data_response");
        quit_handler(this);
        return;
    }
    
    std::string data(boost::asio::buffer_cast<const char*>(response.data()),bytes_transferred-5);
    LOG4CXX_DEBUG(logger, "Got data "<<data);       
    mail_message.body = data;
    response.consume(bytes_transferred);
    std::ostream request_stream(&request);    
    request_stream << "250 Ok\r\n";    
    boost::asio::async_write(socket,request,
                             [this](const boost::system::error_code& error, size_t bytes_transferred){
        handle_read_commands(error,bytes_transferred);
    });
}


void session::handle_write_quit_command(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading quit command "<<error.message());
        quit_handler(this);
        return;
    }
    response.consume(bytes_transferred);
    std::ostream request_stream(&request);    
    request_stream << "221 Bye\r\n";    
    boost::asio::async_write(socket,request,
                             [this](const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/){
        //done
        quit_handler(this);
    });

}
