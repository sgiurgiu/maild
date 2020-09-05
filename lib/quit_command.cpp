#include "quit_command.h"
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

namespace maild {


quit_command::quit_command(maild_socket& socket):
    smtp_command(socket)
{
}
void quit_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    buffer.consume(buffer.size());
    std::ostream output(&write_buffer);
    output << "221 Bye\r\n";
    socket.write(write_buffer,complete_handler);
}

} // namespace maild
