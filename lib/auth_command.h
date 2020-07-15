#ifndef MAILD_AUTH_COMMAND_H
#define MAILD_AUTH_COMMAND_H

#include "smtp_command.h"
#include <log4cxx/logger.h>

namespace maild {

class auth_command: public smtp_command
{
public:
    auth_command(boost::asio::ip::tcp::socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;
private:
    void write_plain_ok_response(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void read_plain_username_password_response(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void write_authentication_successful(complete_handler_t complete_handler);
    void write_login_ask_for_password(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void read_login_password(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void write_login_ask_for_username(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void read_login_username(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    static log4cxx::LoggerPtr logger;

};

} // namespace maild

#endif // MAILD_AUTH_COMMAND_H
