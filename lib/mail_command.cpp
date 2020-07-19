#include "mail_command.h"
#include "utils.h"

#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <spdlog/spdlog.h>

namespace maild {

mail_command::mail_command(boost::asio::ip::tcp::socket& socket,mail& mail_message):smtp_command(socket),
    mail_message(mail_message)
{

}
void mail_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    mail_message.from = utils::get_mail_from(line);
    spdlog::debug( "Got mail from: {}",mail_message.from);
    std::ostream output(&write_buffer);
    output << "250 Ok\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}
} // namespace maild
