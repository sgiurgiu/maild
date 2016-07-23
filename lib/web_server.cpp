#include "web_server.h"
#include "web_server_impl.h"

using namespace maild;

web_server::web_server(const web_options& options):impl(new web_server_impl(options))
{
}
maild::web_server::~web_server()
{
    delete impl;
}

void web_server::set_options(const web_options& options)
{
    impl->set_options(options);
    
}
void web_server::run()
{        
    impl->run();
}
