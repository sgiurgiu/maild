/*
 * Could not load previous license
 */

#ifndef MAILD_WEB_FILE_SERVER_H
#define MAILD_WEB_FILE_SERVER_H

#include <string>
#include <memory>
#include <boost/beast.hpp>
#include <filesystem>

namespace maild {
    
class magic_handler;

class web_file_server
{
public:
    web_file_server(const std::string& path);
    ~web_file_server();
    boost::beast::http::response<boost::beast::http::string_body>
    get_file_contents(const std::string& file);
        
private:
    std::filesystem::path get_file(const std::string& file);
    std::string get_mime_type(const std::string& file);
private:
    std::string path;    
    std::unique_ptr<magic_handler> magic;
};
}

#endif // MAILD_WEB_FILE_SERVER_H
