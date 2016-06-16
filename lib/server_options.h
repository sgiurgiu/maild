#ifndef MAILD_SERVER_OPTIONS_H
#define MAILD_SERVER_OPTIONS_H

#include <istream>
#include <set>
#include <string>

namespace maild {

class server_options
{
public:
    server_options();
    void load(std::istream& conf_stream);
    int get_plain_port() const;
    std::string get_db_connection_string() const;
    std::string get_domain_name() const;
    std::set<std::string> get_ips() const;
    int64_t get_keep_mail_seconds() const;
    int64_t get_check_mail_interval_seconds() const;
private:    
    int64_t keep_mail_seconds = 60;
    int64_t check_mail_interval_seconds = 5;
    int plain_port = 25;
    int secure_port = 465;
    bool plain_enabled = true;
    bool secure_enabled = false;
    std::string domain_name = "sergiu-pc.zergiu.com";
    std::set<std::string> ips = {"127.0.0.1"};
    std::string db_connection_string = "postgresql://maild:maild@localhost/maild";
};
}

#endif // MAILD_SERVER_OPTIONS_H
