#ifndef MAILD_SMTP_COMMAND_H
#define MAILD_SMTP_COMMAND_H

#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>

namespace maild {
using complete_handler_t = std::function<void(const boost::system::error_code&, std::size_t)>;

class smtp_command
{
public:  
    smtp_command(boost::asio::ip::tcp::socket& socket);

    virtual void execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler) = 0;
    virtual ~smtp_command();
protected:
    complete_handler_t complete_handler;
    boost::asio::ip::tcp::socket& socket;
    boost::asio::streambuf write_buffer;
};

} // namespace maild

#endif // MAILD_MAIL_COMMAND_H
