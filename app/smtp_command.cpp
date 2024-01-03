#include "smtp_command.h"

namespace maild {

smtp_command::smtp_command(maild_socket& socket):socket(socket)
{
}
smtp_command::~smtp_command()
{}

} // namespace maild
