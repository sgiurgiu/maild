#ifndef MAILD_HELP_COMMAND_H
#define MAILD_HELP_COMMAND_H

#include "smtp_command.h"

namespace maild {


class help_command: public smtp_command
{
public:
    help_command(maild_socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;
};


} // namespace maild

#endif // HELP_COMMAND_H
