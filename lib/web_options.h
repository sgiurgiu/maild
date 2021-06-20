#ifndef MAILD_WEB_OPTIONS_H
#define MAILD_WEB_OPTIONS_H

#include <istream>
#include <set>
#include <string>

namespace maild {

struct web_certificate
{
    std::string certificate_chain;
    std::string private_key;
    std::string dh_file;
};

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
    web_certificate get_web_certificate() const;
    bool is_ssl() const;
private:    
    int port = 8080;
    std::string domain_name = "sergiu-pc.zergiu.com";
    std::set<std::string> ips = {"127.0.0.1"};
    std::string db_connection_string = "postgresql://maild:maild@localhost/maild";
    std::string files_dir = "www";
    std::string api_prefix = "/api";
    bool ssl = false;
    web_certificate certificate;
};
}

#endif // MAILD_SERVER_OPTIONS_H
