#ifndef MAILD_SERVER_OPTIONS_H
#define MAILD_SERVER_OPTIONS_H

#include <istream>
#include <set>
#include <vector>
#include <string>

namespace maild {

struct server
{
    std::string ip = "127.0.0.1";
    int port = 25;
    bool enabled = true;
    bool ssl = false;
};

struct certificates
{
    std::string certificate_chain;
    std::string private_key;
    std::string dh_file;
};

class server_options
{
public:
    server_options();
    void load(std::istream& conf_stream);    
    std::string get_db_connection_string() const;
    std::string get_domain_name() const;
    certificates get_certificates() const;
    int64_t get_keep_mail_seconds() const;
    int64_t get_check_mail_interval_seconds() const;
    std::vector<server> get_servers() const;
private:    
    int64_t keep_mail_seconds = 60;
    int64_t check_mail_interval_seconds = 5;
    std::string domain_name = "sergiu-pc.zergiu.com";
    std::string db_connection_string = "postgresql://maild:maild@localhost/maild";
    certificates certificate_files;
    std::vector<server> servers;
};
}

#endif // MAILD_SERVER_OPTIONS_H
