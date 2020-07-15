#ifndef MAILD_DATA_COMMAND_H
#define MAILD_DATA_COMMAND_H

#include "smtp_command.h"
#include "mail.h"
#include <log4cxx/logger.h>

namespace maild {

class data_command: public smtp_command
{
public:
    data_command(boost::asio::ip::tcp::socket& socket,mail& mail_message);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;

private:
    void handle_data_written_command(complete_handler_t complete_handler,
                                  boost::asio::streambuf& buffer,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_transferred);
    void handle_data_read_command(complete_handler_t complete_handler,
                                  boost::asio::streambuf& buffer,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_transferred);
private:
    static log4cxx::LoggerPtr logger;
    mail& mail_message;
};

} // namespace maild

#endif // MAILD_DATA_COMMAND_H
