#ifndef MAILD_STARTTLS_COMMAND_H
#define MAILD_STARTTLS_COMMAND_H

#include "smtp_command.h"
#include <map>
#include <memory>

namespace maild {

class starttls_command : public smtp_command
{
public:
    starttls_command(maild_socket& socket,std::map<std::string,std::unique_ptr<smtp_command>>* commands);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;
private:
    void handle_starttls_written_command(complete_handler_t complete_handler,
                                  boost::asio::streambuf& buffer,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_transferred);

private:
    std::map<std::string,std::unique_ptr<smtp_command>>* commands;
};

} // namespace maild

#endif // MAILD_STARTTLS_COMMAND_H
