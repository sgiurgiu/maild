#ifndef MAILD_AUTH_COMMAND_H
#define MAILD_AUTH_COMMAND_H

#include "smtp_command.h"

namespace maild {

class auth_command: public smtp_command
{
public:
    auth_command(maild_socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;
private:
    void write_plain_ok_response(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void read_plain_username_password_response(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void write_authentication_successful(complete_handler_t complete_handler);
    void write_login_ask_for_password(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void read_login_password(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void write_login_ask_for_username(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
    void read_login_username(boost::asio::streambuf& buffer,complete_handler_t complete_handler);
};

} // namespace maild

#endif // MAILD_AUTH_COMMAND_H
