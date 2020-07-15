#ifndef MAILD_HELLO_COMMAND_H
#define MAILD_HELLO_COMMAND_H

#include "smtp_command.h"
#include <log4cxx/logger.h>

namespace maild {

class hello_command : public smtp_command
{
public:
    hello_command(boost::asio::ip::tcp::socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;

private:
    static log4cxx::LoggerPtr logger;
};

} // namespace maild

#endif // MAILD_HELLO_COMMAND_H
