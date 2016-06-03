#ifndef MAILD_SERVER_OPTIONS_H
#define MAILD_SERVER_OPTIONS_H

#include <istream>
#include <vector>
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
private:    
    int plain_port = 2525;
    int secure_port = 4655;
    bool plain_enabled = true;
    bool secure_enable = false;    
    std::string domain_name = "sergiu-pc.zergiu.com";
    std::vector<std::string> ips = {"127.0.0.1"};
    std::string db_connection_string = "postgresql://maild:maild@localhost/maild";
};
}

#endif // MAILD_SERVER_OPTIONS_H
