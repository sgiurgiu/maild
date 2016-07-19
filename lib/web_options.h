#ifndef MAILD_WEB_OPTIONS_H
#define MAILD_WEB_OPTIONS_H

#include <istream>
#include <set>
#include <string>

namespace maild {

class web_options
{
public:
    web_options();
    void load(std::istream& conf_stream);
    int get_port() const;
    std::string get_db_connection_string() const;
    std::string get_domain_name() const;
    std::set<std::string> get_ips() const;
    std::string get_files_dir() const;
    std::string get_api_prefix() const;
private:    
    int port = 8080;
    std::string domain_name = "sergiu-pc.zergiu.com";
    std::set<std::string> ips = {"127.0.0.1"};
    std::string db_connection_string = "postgresql://maiaald:maild@localhost/maild";
    std::string files_dir = "www";
    std::string api_prefix = "/api";
};
}

#endif // MAILD_SERVER_OPTIONS_H
