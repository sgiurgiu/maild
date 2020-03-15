#ifndef MAILD_WEB_API_SERVER_H
#define MAILD_WEB_API_SERVER_H

#include <string>
#include <pqxx/connection>
#include <log4cxx/logger.h>
#include <boost/beast.hpp>

namespace mimetic {
    class MimeEntity;
}
namespace maild {

class web_api_server
{
private:
    using response = boost::beast::http::response<boost::beast::http::string_body>;
    using request = boost::beast::http::request<boost::beast::http::string_body>;
public:
    web_api_server();
    web_api_server(const std::string& db_conn_string);
    response get_users_mails(const request& request, const std::string& user);
    response get_mail(const request& request,int id,const std::string& type);
private:
    pqxx::connection db;
    static log4cxx::LoggerPtr logger;

};
}

#endif // MAILD_WEB_API_SERVER_H
