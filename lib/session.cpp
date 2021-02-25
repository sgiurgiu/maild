#include "session.h"
#include "utils.h"

#include <string>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <pqxx/transaction>

#include "hello_command.h"
#include "ehlo_command.h"
#include "mail_command.h"
#include "rcpt_command.h"
#include "data_command.h"
#include "quit_command.h"
#include "auth_command.h"
#include "rset_command.h"
#include "help_command.h"
#include "verify_command.h"
#include "starttls_command.h"

#include <spdlog/spdlog.h>

using namespace maild;

session::session(const boost::asio::any_io_executor& executor,
                 const std::string& db_connection_string,const std::string& domain_name,
                 const certificates& certificate_files, bool is_fully_ssl)
                : db_connection_string(db_connection_string),domain_name(domain_name),
                  strand(boost::asio::make_strand(executor)),
                  socket(strand,certificate_files),
                  session_start(std::chrono::steady_clock::now()),is_fully_ssl(is_fully_ssl),
                  timer(executor)
{
    commands["HELO"] = std::make_unique<hello_command>(socket);
    commands["EHLO"] = std::make_unique<ehlo_command>(socket,domain_name);
    commands["MAIL"] = std::make_unique<mail_command>(socket,mail_message);
    commands["RCPT"] = std::make_unique<rcpt_command>(socket,mail_message);
    commands["DATA"] = std::make_unique<data_command>(socket,mail_message);
    commands["QUIT"] = std::make_unique<quit_command>(socket);
    commands["AUTH"] = std::make_unique<auth_command>(socket);
    commands["RSET"] = std::make_unique<rset_command>(socket);
    commands["NOOP"] = std::make_unique<rset_command>(socket);
    commands["HELP"] = std::make_unique<help_command>(socket);
    commands["VRFY"] = std::make_unique<verify_command>(socket);
    commands["STARTTLS"] = std::make_unique<starttls_command>(socket,&commands);    
}

session::~session()
{
    spdlog::debug("Deleting session");
    spdlog::default_logger()->flush();
}

boost::asio::ip::tcp::socket& session::get_socket()
{
    return socket.get_socket();
}

mail session::get_mail_message() const
{
  return mail_message;
}
void session::start()
{
    timer.expires_after(std::chrono::minutes(1));
    timer.async_wait(std::bind(&session::close_socket_timeout,this,std::placeholders::_1));

    socket.set_ssl(is_fully_ssl);
    if(is_fully_ssl)
    {
        commands.erase("STARTTLS");//we no longer have that command
        reinterpret_cast<ehlo_command*>(commands["EHLO"].get())->set_starttls_enabled(false);
    }
    on_start();
}
void session::on_start()
{
    using namespace std::placeholders;

    std::ostream request_stream(&request);
    std::string greeting_message = "220 "+domain_name+" ESMTP MailD ready";
    request_stream << greeting_message << "\r\n";
    spdlog::debug("Starting session, writing {} socket open:{}, fully ssl:{}",
                  greeting_message,socket.is_open(),is_fully_ssl);

    socket.write(request,std::bind(&session::handle_read_commands,shared_from_this(),_1,_2));
}

void session::handle_read_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        spdlog::error( "Error reading command {}",error.message());
        return;
    }
    using namespace std::placeholders;
    request.consume(bytes_transferred);
    socket.read_until(response,"\r\n",std::bind(&session::handle_parse_commands,shared_from_this(),_1,_2));
}
void session::handle_parse_commands(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if(error)
    {
        spdlog::error( "Error reading command {}",error.message());
        return;
    }
    if(bytes_transferred <= 5)
    {
        spdlog::error( "Error reading greeting, with {} bytes transfered in handle_write_commands",bytes_transferred);
        return;
    }
    using namespace std::placeholders;
    auto handle_read_commands_func = std::bind(&session::handle_read_commands,shared_from_this(),_1,_2);

    std::istream input(&response);
    std::string command;
    input >> command;
    std::transform(command.begin(), command.end(), command.begin(),[](unsigned char c){return std::toupper(c);});
    spdlog::debug( "Got command {}",command);
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
        spdlog::error( "Command {} not implemented",command);
        response.consume(bytes_transferred);
        std::ostream request_stream(&request);
        request_stream << "502 Command not implemented\r\n";
        socket.write(request,handle_read_commands_func);
    }
}

void session::handle_complete_quit_command(const boost::system::error_code& /*error*/, std::size_t /*bytes_transferred*/)
{
    //save message
    spdlog::debug( "Saving message");
    try
    {
        pqxx::connection db(db_connection_string);
        db.prepare("new_mail","insert into mails(from_address,to_address,body,date_received,username) values ($1,$2,$3,NOW(),$4)");
        //deadlock possible apparently. we're low load so we're fine
        db.prepare("mail_count_increment","update counters set counter=counter+1 where id='mails_received'");
        pqxx::work w(db);
        pqxx::binarystring blob(mail_message.body);
        for(const auto& to : mail_message.to)
        {            
            std::string username = "";
            auto at_index = to.find('@');
            if(at_index != to.npos) {
                username = to.substr(0,at_index);
            }
            w.exec_prepared("new_mail",mail_message.from,to,blob,
                username);
        }
        w.exec_prepared("mail_count_increment");
        w.commit();
    }
    catch(const std::exception& ex)
    {
        spdlog::error("CANNOT SAVE MAIL: {}",ex.what());
    }

    spdlog::default_logger()->flush();
}

void session::close_socket_timeout(const boost::system::error_code& error)
{    
    if(!error)
    {
        spdlog::debug("Closing socket. Timer expired");
        socket.close();
    }
    else
    {
        spdlog::error( "Timeout invoked because {} : {}",error.value(),error.message());
    }
}
