#include "rset_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

namespace maild {

rset_command::rset_command(boost::asio::ip::tcp::socket& socket):
    smtp_command(socket)
{

}
void rset_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    buffer.consume(buffer.size());
    spdlog::debug("Got RESET command");

    std::ostream output(&write_buffer);
    output << "250 Ok\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}

} // namespace maild
