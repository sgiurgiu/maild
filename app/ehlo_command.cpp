#include "ehlo_command.h"
#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>
namespace maild {

ehlo_command::ehlo_command(maild_socket& socket, const std::string &domain_name):
    smtp_command(socket),domain_name(domain_name)
{

}
void ehlo_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    spdlog::debug("Got hello and client name: {}",line);

    std::ostream output(&write_buffer);
    output << "250-"<< domain_name <<" Hello " << line << "\r\n";
    output << "250-AUTH LOGIN PLAIN\r\n";
    output << "250-SIZE 1000000\r\n";
    output << "250-HELP\r\n";
    output << "250-RSET\r\n";
    output << "250-NOOP\r\n";
    if(starttls_enabled)
    {
        output << "250-STARTTLS\r\n";
    }
    output << "250 VRFY\r\n";

    socket.write(write_buffer,complete_handler);
}

} // namespace maild
