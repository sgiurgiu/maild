#include "server_options.h"

using namespace maild;

server_options::server_options()
{

}
void server_options::load(std::istream& conf_stream)
{
    
}

int server_options::get_plain_port() const
{
    return plain_port;
}
