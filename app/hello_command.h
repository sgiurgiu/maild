#ifndef MAILD_HELLO_COMMAND_H
#define MAILD_HELLO_COMMAND_H

#include "smtp_command.h"

namespace maild {

class hello_command : public smtp_command
{
public:
    hello_command(maild_socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;


};

} // namespace maild

#endif // MAILD_HELLO_COMMAND_H
