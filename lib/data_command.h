#ifndef MAILD_DATA_COMMAND_H
#define MAILD_DATA_COMMAND_H

#include "smtp_command.h"
#include "mail.h"

namespace maild {

class data_command: public smtp_command
{
public:
    data_command(maild_socket& socket,mail& mail_message);
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
    mail& mail_message;
};

} // namespace maild

#endif // MAILD_DATA_COMMAND_H
