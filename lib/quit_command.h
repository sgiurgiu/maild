#ifndef MAILD_QUIT_COMMAND_H
#define MAILD_QUIT_COMMAND_H

#include "smtp_command.h"

namespace maild {

class quit_command: public smtp_command
{
public:
    quit_command(maild_socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;

};

} // namespace maild

#endif // MAILD_QUIT_COMMAND_H
