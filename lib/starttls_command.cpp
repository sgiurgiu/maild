#include "starttls_command.h"
#include "ehlo_command.h"

namespace maild {

starttls_command::starttls_command(maild_socket& socket,
                                   std::map<std::string,std::unique_ptr<smtp_command>>* commands):
    smtp_command(socket),commands(commands)
{

}
void starttls_command::execute(boost::asio::streambuf& buffer,complete_handler_t complete_handler)
{
    buffer.consume(buffer.size());
    spdlog::debug("Got STARTTLS command");
    std::ostream output(&write_buffer);
    output << "220 Ready to start TLS\r\n";
    socket.write(write_buffer,[this,&buffer,complete_handler]
                 (const boost::system::error_code& error,
                 std::size_t bytes_transferred){
        handle_starttls_written_command(complete_handler,buffer,error,bytes_transferred);
    });
}

void starttls_command::handle_starttls_written_command(complete_handler_t complete_handler,
                              boost::asio::streambuf&,
                              const boost::system::error_code& error,
                              std::size_t bytes_transferred)
{
    if(error)
    {
        spdlog::error("Error writing starttls command {}",error.message());
        return;
    }
    write_buffer.consume(bytes_transferred);
    socket.set_ssl(true);
    if(commands && (*commands)["EHLO"])
    {
        auto ehlo = reinterpret_cast<ehlo_command*>((*commands)["EHLO"].get());
        ehlo->set_starttls_enabled(false);
    }
    complete_handler(error,bytes_transferred);
}

} // namespace maild
