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
    template <typename Router>
    void fill_router_routes(Router& router, web_file_server &file_server);
    void start_lisening(boost::asio::io_context& ioc);
private:
    web_options options;
    boost::asio::ssl::context ssl_context;
    std::string apiMailContentRegex = {R"(^/api/mail/(\d+)/(raw|html|text)$)"};
    std::string apiMailsRegex = {R"(^/api/mails/(.+)$)"};
    std::string indexHtmlRegex = {R"(^/$)"};
    std::string filesRegex = {R"(^/(?:(?!api/)[a-zA-Z_\s\-\.\/])+$)"};
    std::string everythingElseRegex = {R"(^.*$)"};
};

}

#endif // MAILD_WEB_SERVER_H
