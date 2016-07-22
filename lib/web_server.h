#ifndef MAILD_WEB_SERVER_H
#define MAILD_WEB_SERVER_H

#include "web_options.h"

namespace maild {
class web_server_impl;
class web_server
{
public:
    web_server();
    web_server(const web_options& options);
    ~web_server();
    void set_options(const web_options& options);
    void run();
private:    
    web_server_impl* impl;
};

}

#endif // MAILD_WEB_SERVER_H
