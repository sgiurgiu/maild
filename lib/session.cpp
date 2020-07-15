#include "session.h"
#include "utils.h"

#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <pqxx/connection>
#include <pqxx/transaction>

#include "hello_command.h"
#include "ehlo_command.h"
#include "mail_command.h"
#include "rcpt_command.h"
#include "data_command.h"
#include "quit_command.h"
#include "auth_command.h"

using namespace maild;

log4cxx::LoggerPtr session::logger(log4cxx::Logger::getLogger("session"));

session::session(boost::asio::io_context& io_context,
                 const server_options& options)
                : options(options),strand(boost::asio::make_strand(io_context)), socket(strand),
                  session_start(std::chrono::steady_clock::now())
{
    commands["HELO"] = std::make_unique<hello_command>(socket);
    commands["EHLO"] = std::make_unique<ehlo_command>(socket,options.get_domain_name());
    commands["MAIL"] = std::make_unique<mail_command>(socket,mail_message);
    commands["RCPT"] = std::make_unique<rcpt_command>(socket,mail_message);
    commands["DATA"] = std::make_unique<data_command>(socket,mail_message);
    commands["QUIT"] = std::make_unique<quit_command>(socket);
    commands["AUTH"] = std::make_unique<auth_command>(socket);
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

    boost::asio::async_write(socket,request,std::bind(&session::handle_read_commands,shared_from_this(),_1,_2));
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
    boost::asio::async_read_until(socket,response,"\r\n",std::bind(&session::handle_parse_commands,shared_from_this(),_1,_2));
}
void session::handle_parse_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        LOG4CXX_ERROR(logger, "Error reading command "<<error.message());
        return;
    }
    if(bytes_transferred <= 5)
    {
        LOG4CXX_ERROR(logger, "Error reading greeting, with "<<bytes_transferred<<" bytes transfered in handle_write_commands");
        return;
    }
    using namespace std::placeholders;
    auto handle_read_commands_func = std::bind(&session::handle_read_commands,shared_from_this(),_1,_2);

    std::istream input(&response);
    std::string command;
    input >> command;
    std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){return std::toupper(c);});
    LOG4CXX_DEBUG(logger, "Got command "<<command);
    auto& command_executor = commands[command];
    if(command_executor)
    {
        if(command == "QUIT")
        {
            //we just need a different handler for quit
            command_executor->execute(response,std::bind(&session::handle_complete_quit_command,shared_from_this(),_1,_2));
        }
        else
        {
            command_executor->execute(response,handle_read_commands_func);
        }
    }
    else
    {
        LOG4CXX_ERROR(logger, "Command "<< command << " not implemented");
        response.consume(bytes_transferred);
        std::ostream request_stream(&request);
        request_stream << "502 Command not implemented\r\n";
        boost::asio::async_write(socket,request,handle_read_commands_func);
    }
}

void session::handle_complete_quit_command(const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/)
{

    //save message
    if(mail_message.from.empty() || mail_message.to.empty() || mail_message.body.empty()) {
        LOG4CXX_DEBUG(logger, "Not saving message, stuff is empty");
        return;
    }
    LOG4CXX_DEBUG(logger, "Saving message");
    pqxx::connection db(options.get_db_connection_string());
    db.prepare("new_mail","insert into mails(from_address,to_address,body,date_received,username) values ($1,$2,$3,NOW(),$4)");
    std::string username = mail_message.to.substr(0,mail_message.to.find('@'));
    pqxx::work w(db);
    w.exec_prepared("new_mail",mail_message.from,mail_message.to,mail_message.body,
        username);
    w.commit();
}
