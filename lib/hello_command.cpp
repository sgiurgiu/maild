#include "hello_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
namespace maild {

log4cxx::LoggerPtr hello_command::logger(log4cxx::Logger::getLogger("hello_command"));

hello_command::hello_command(boost::asio::ip::tcp::socket& socket):
    smtp_command(socket)
{

}
void hello_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    LOG4CXX_DEBUG(logger, "Got hello and client name: "<<line);

    std::ostream output(&write_buffer);
    output << "250 Hello " << line << ", glad to meet you\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}

} // namespace maild
