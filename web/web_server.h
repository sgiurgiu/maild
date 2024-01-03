#ifndef MAILD_WEB_SERVER_H
#define MAILD_WEB_SERVER_H

#include "web_options.h"
#include "web_file_server.h"
#include <boost/asio/ssl.hpp>

namespace maild {
class web_server
{
public:
    web_server(const web_options& options);
    void run();
private:
    web_options options;
    boost::asio::ssl::context ssl_context;
};

}

#endif // MAILD_WEB_SERVER_H
