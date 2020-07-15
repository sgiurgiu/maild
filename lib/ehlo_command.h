#ifndef MAILD_EHLO_COMMAND_H
#define MAILD_EHLO_COMMAND_H

#include "smtp_command.h"
#include <log4cxx/logger.h>

namespace maild {

class ehlo_command : public smtp_command
{
public:
    ehlo_command(boost::asio::ip::tcp::socket& socket, const std::string& domain_name);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;

private:
    static log4cxx::LoggerPtr logger;
    std::string domain_name;
};

} // namespace maild

#endif // MAILD_EHLO_COMMAND_H
