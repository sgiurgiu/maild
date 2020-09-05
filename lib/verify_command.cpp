#include "verify_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

namespace maild {

verify_command::verify_command(maild_socket& socket):
    smtp_command(socket)
{

}
void verify_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    spdlog::debug("Got verify command for mail : {}",line);

    std::ostream output(&write_buffer);
    output << "250 Ok\r\n"; //we verify everyone
    socket.write(write_buffer,complete_handler);
}

} // namespace maild
