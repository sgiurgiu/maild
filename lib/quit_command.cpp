#include "quit_command.h"
#include <ostream>
#include <boost/asio/write.hpp>

namespace maild {
log4cxx::LoggerPtr quit_command::logger(log4cxx::Logger::getLogger("quit_command"));

quit_command::quit_command(boost::asio::ip::tcp::socket& socket):
    smtp_command(socket)
{
}
void quit_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    buffer.consume(buffer.size());
    std::ostream output(&write_buffer);
    output << "221 Bye\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}

} // namespace maild
