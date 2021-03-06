#include "hello_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

namespace maild {

hello_command::hello_command(maild_socket& socket):
    smtp_command(socket)
{

}
void hello_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    spdlog::debug("Got hello and client name: {}",line);

    std::ostream output(&write_buffer);
    output << "250 Hello " << line << ", glad to meet you\r\n";
    socket.write(write_buffer,complete_handler);
}

} // namespace maild
