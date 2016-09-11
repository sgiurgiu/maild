#ifndef MAILD_WEB_SERVER_H
#define MAILD_WEB_SERVER_H

#include "web_options.h"
#include <log4cxx/logger.h>

namespace maild {
class web_server
{
public:
    web_server(const web_options& options);
    void run();
private:    
private:
    web_options options;
    static log4cxx::LoggerPtr logger;
};

}

#endif // MAILD_WEB_SERVER_H
