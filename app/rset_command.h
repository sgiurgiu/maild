#ifndef MAILD_RSET_COMMAND_H
#define MAILD_RSET_COMMAND_H

#include "smtp_command.h"

namespace maild {

class rset_command: public smtp_command
{
public:
    rset_command(maild_socket& socket);
    void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) override;
};

} // namespace maild

#endif // MAILD_RSET_COMMAND_H
