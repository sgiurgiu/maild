#ifndef MAILD_WEB_API_SERVER_H
#define MAILD_WEB_API_SERVER_H

#include <string>
#include <pqxx/connection>
#include <log4cxx/logger.h>
#include <vector>

namespace crow {
    struct response;
}
namespace mimetic {
    class MimeEntity;
}
namespace maild {

class web_api_server
{
public:
    web_api_server(const std::string& db_conn_string);
    crow::response get_users_mails(const std::string& user);
    crow::response get_mail(int id);
    std::string get_part(mimetic::MimeEntity* me,const std::vector<std::string>& types);
private:
    pqxx::connection db;
    static log4cxx::LoggerPtr logger;

};
}

#endif // MAILD_WEB_API_SERVER_H
