#include "data_command.h"
#include "utils.h"

#include <istream>
#include <ostream>
#include <boost/asio/write.hpp>
#include <boost/asio/read_until.hpp>

#include <spdlog/spdlog.h>

namespace maild {

data_command::data_command(boost::asio::ip::tcp::socket& socket,mail& mail_message):smtp_command(socket),
    mail_message(mail_message)
{
}
void data_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    buffer.consume(buffer.size());
    std::ostream output(&write_buffer);
    output << "354 End data with <CR><LF>.<CR><LF>\r\n";
    boost::asio::async_write(socket,write_buffer,[this,&buffer,complete_handler]
                             (const boost::system::error_code& error,
                             std::size_t bytes_transferred){
        handle_data_written_command(complete_handler,buffer,error,bytes_transferred);
    });
}
void data_command::handle_data_written_command(complete_handler_t complete_handler,
                                            boost::asio::streambuf& buffer,
                                            const boost::system::error_code& error,
                                            std::size_t bytes_transferred)
{
    if(error)
    {
        spdlog::error("Error reading data command {}",error.message());
        return;
    }
    write_buffer.consume(bytes_transferred);

    boost::asio::async_read_until(socket,buffer,"\r\n.\r\n",[this,&buffer,complete_handler]
                                  (const boost::system::error_code& error,
                                  std::size_t bytes_transferred){
        handle_data_read_command(complete_handler,buffer,error,bytes_transferred);
    });
}

void data_command::handle_data_read_command(complete_handler_t complete_handler,
                              boost::asio::streambuf& buffer,
                              const boost::system::error_code& error,
                              std::size_t bytes_transferred)
{
    if(error)
    {
        spdlog::error("Error reading data contents ",error.message());
        return;
    }
    if(bytes_transferred <= 6)
    {
        spdlog::error( "Error reading greeting, with {} bytes transfered in handle_write_data_response",bytes_transferred);
        return;
    }
    std::string data(boost::asio::buffer_cast<const char*>(buffer.data()),bytes_transferred-5);
    spdlog::error( "Got data {}",data);
    mail_message.body = data;
    buffer.consume(bytes_transferred);
    std::ostream request_stream(&write_buffer);
    request_stream << "250 Ok\r\n";
    boost::asio::async_write(socket,write_buffer,complete_handler);
}
} // namespace maild
