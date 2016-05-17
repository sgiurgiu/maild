#ifndef MAILD_SERVER_MANAGER_H
#define MAILD_SERVER_MANAGER_H

#include "server_options.h"

namespace maild {

class server_manager
{
public:
    server_manager(const server_options& options);
    void run();
private:
    server_options options;    
};

}

#endif // MAILD_SERVER_MANAGER_H
