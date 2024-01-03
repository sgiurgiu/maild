#include "rset_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

namespace maild {

rset_command::rset_command(maild_socket& socket):
    smtp_command(socket)
{

}
void rset_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    buffer.consume(buffer.size());
    spdlog::debug("Got RESET command");

    std::ostream output(&write_buffer);
    output << "250 Ok\r\n";
    socket.write(write_buffer,complete_handler);
}

} // namespace maild
