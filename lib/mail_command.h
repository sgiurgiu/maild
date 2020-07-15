#ifndef MAILD_MAIL_COMMAND_H
#define MAILD_MAIL_COMMAND_H

#include "smtp_command.h"
#include "mail.h"
#include <log4cxx/logger.h>

namespace maild {

class mail_command: public smtp_command
{
public:
    mail_command(boost::asio::ip::tcp::socket& socket,mail& mail_message);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;
private:
    static log4cxx::LoggerPtr logger;
    mail& mail_message;
};

} // namespace maild

#endif // MAILD_MAIL_COMMAND_H
