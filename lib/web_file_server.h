/*
 * Could not load previous license
 */

#ifndef MAILD_WEB_FILE_SERVER_H
#define MAILD_WEB_FILE_SERVER_H

#include <string>
#include <log4cxx/logger.h>
#include <memory>

namespace boost {
    namespace filesystem {
        class path;
    }
}
namespace crow {
    struct response;
}
namespace maild {
    
class magic_handler;

class web_file_server
{
public:
    web_file_server(const std::string& path);
    ~web_file_server();
    crow::response get_file_contents(const std::string& file);
        
private:
    boost::filesystem::path get_file(const std::string& file);
    std::string get_mime_type(const std::string& file);
private:
    std::string path;    
    std::unique_ptr<magic_handler> magic;
    static log4cxx::LoggerPtr logger;
};
}

#endif // MAILD_WEB_FILE_SERVER_H
