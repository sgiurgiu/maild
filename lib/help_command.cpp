#include "help_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>
namespace maild {


help_command::help_command(boost::asio::ip::tcp::socket& socket):
    smtp_command(socket)
{

}
void help_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    spdlog::debug("Got help command for command : {}",line);

    std::ostream output(&write_buffer);
    output << "211 Help not implemented. Go check an RFC\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}

}//namespace maild
