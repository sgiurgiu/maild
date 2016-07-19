#ifndef MAILD_WEB_SERVER_IMPL_H
#define MAILD_WEB_SERVER_IMPL_H

#include "web_options.h"
#include "web_server.h"
#include "crow.h"




#include <log4cxx/logger.h>
#include <memory>

namespace maild {

class web_server_impl
{
friend class web_server;   
private:
    web_server_impl();
    ~web_server_impl() = default;
    web_server_impl(const web_options& options);
    void set_options(const web_options& options);
    void run();
    void stop();
private:
    web_options options;
    std::unique_ptr<crow::SimpleApp> app;
    static log4cxx::LoggerPtr logger;
};
}

#endif // MAILD_WEB_SERVER_IMPL_H
