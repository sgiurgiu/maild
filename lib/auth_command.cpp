#include "auth_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/algorithm/string.hpp>
namespace maild {
log4cxx::LoggerPtr auth_command::logger(log4cxx::Logger::getLogger("auth_command"));
auth_command::auth_command(boost::asio::ip::tcp::socket& socket):
    smtp_command(socket)
{
}
void auth_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string auth_method;
    input >> auth_method;
    std::transform(auth_method.begin(), auth_method.end(), auth_method.begin(),[](unsigned char c){return std::toupper(c);});
    LOG4CXX_DEBUG(logger, "Got auth method: "<<auth_method);
    //get the rest of the line if any
    std::string potential_name;
    std::getline(input,potential_name);
    boost::algorithm::trim(potential_name);
    if(potential_name.empty())
    {
        if(auth_method == "PLAIN")
        {
            write_plain_ok_response(buffer,complete_handler);
        }
        else if(auth_method == "LOGIN")
        {
            write_login_ask_for_username(buffer,complete_handler);
        }
    }
    else
    {
        if(auth_method == "PLAIN")
        {
            write_authentication_successful(complete_handler);
        }
        else if(auth_method == "LOGIN")
        {
            write_login_ask_for_password(buffer,complete_handler);
        }
    }
}
void auth_command::write_plain_ok_response(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::ostream output(&write_buffer);
    output <<  "334\r\n";
    boost::asio::async_write(socket,write_buffer,[this,&buffer,complete_handler]
                             (const boost::system::error_code& error, size_t bytes_transferred){
        write_buffer.consume(bytes_transferred);
        if(!error)
        {
            read_plain_username_password_response(buffer,complete_handler);
        }
    });
}
void auth_command::read_plain_username_password_response(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    boost::asio::async_read_until(socket,buffer,"\r\n",[this,&buffer,complete_handler]
                                  (const boost::system::error_code& error, size_t bytes_transferred){
        buffer.consume(bytes_transferred);
        if(!error)
        {
            write_authentication_successful(complete_handler);
        }
    });
}
void auth_command::write_login_ask_for_password(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::ostream output(&write_buffer);
    output <<  "334 UGFzc3dvcmQ6\r\n";
    boost::asio::async_write(socket,write_buffer,[this,&buffer,complete_handler]
                             (const boost::system::error_code& error, size_t bytes_transferred){
        write_buffer.consume(bytes_transferred);
        if(!error)
        {
            read_login_password(buffer,complete_handler);
        }
    });
}
void auth_command::read_login_password(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    boost::asio::async_read_until(socket,buffer,"\r\n",[this,&buffer,complete_handler]
                                  (const boost::system::error_code& error, size_t bytes_transferred){
        buffer.consume(bytes_transferred);
        if(!error)
        {
            write_authentication_successful(complete_handler);
        }
    });
}
void auth_command::write_login_ask_for_username(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::ostream output(&write_buffer);
    output <<  "334 VXNlcm5hbWU6\r\n";
    boost::asio::async_write(socket,write_buffer,[this,&buffer,complete_handler]
                             (const boost::system::error_code& error, size_t bytes_transferred){
        write_buffer.consume(bytes_transferred);
        if(!error)
        {
            read_login_username(buffer,complete_handler);
        }
    });
}
void auth_command::read_login_username(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    boost::asio::async_read_until(socket,buffer,"\r\n",[this,&buffer,complete_handler]
                                  (const boost::system::error_code& error, size_t bytes_transferred){
        buffer.consume(bytes_transferred);
        if(!error)
        {
            write_login_ask_for_password(buffer,complete_handler);
        }
    });
}

void auth_command::write_authentication_successful(complete_handler_t complete_handler)
{
    std::ostream output(&write_buffer);
    output << "235 2.7.0 Authentication successful\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}

} // namespace maild
