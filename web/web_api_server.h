#ifndef MAILD_WEB_API_SERVER_H
#define MAILD_WEB_API_SERVER_H

#include <string>
#include <pqxx/connection>
#include <boost/beast.hpp>

namespace mimetic {
    class MimeEntity;
}
namespace maild {

class web_api_server
{
private:
    using response = boost::beast::http::response<boost::beast::http::string_body>;
public:
    web_api_server();
    web_api_server(const std::string& db_conn_string);
    response get_users_mails(const std::string& user);
    response get_mail(int id,const std::string& type);
private:
    pqxx::connection db;
};
}

#endif // MAILD_WEB_API_SERVER_H
