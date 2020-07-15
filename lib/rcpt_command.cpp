#include "rcpt_command.h"
#include "utils.h"

#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>

namespace maild {
log4cxx::LoggerPtr rcpt_command::logger(log4cxx::Logger::getLogger("rcpt_command"));
rcpt_command::rcpt_command(boost::asio::ip::tcp::socket& socket,mail& mail_message):smtp_command(socket),
    mail_message(mail_message)
{

}
void rcpt_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    std::istream input(&buffer);
    std::string line;
    std::getline(input,line);
    mail_message.to = utils::get_mail_to(line);
    LOG4CXX_DEBUG(logger, "Got mail to: "<<mail_message.to);
    std::ostream output(&write_buffer);
    output << "250 Ok\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}


} // namespace maild
