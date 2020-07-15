#include "smtp_command.h"

namespace maild {

smtp_command::smtp_command(boost::asio::ip::tcp::socket& socket):socket(socket)
{
}
smtp_command::~smtp_command()
{}

} // namespace maild
