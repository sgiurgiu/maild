#include "web_file_server.h"
#include "web_not_found_exception.h"
#include "magic_handler.h"

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wpedantic"
#include "crow_all.h"

#pragma GCC diagnostic pop


#include <sstream>
#include <boost/filesystem.hpp>

using namespace maild;
log4cxx::LoggerPtr web_file_server::logger(log4cxx::Logger::getLogger("web_file_server"));

web_file_server::web_file_server(const std::string& path):path(path),magic(std::make_unique<magic_handler>())
{
}
web_file_server::~web_file_server() = default;

crow::response web_file_server::get_file_contents(const std::string& file)
{
    crow::response rsp;
    try{
        std::string file_name = get_file(file).string();
        std::ifstream in (file_name);   
        rsp.code = 200;
        std::string contents = (static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str());
        rsp.set_header("Content-Length",std::to_string(contents.length()));        
        std::string mime = get_mime_type(file_name);
        LOG4CXX_DEBUG(logger, "file: "<<file<<" has mime "<<mime);
        rsp.set_header("Content-Type",mime);
        rsp.write(contents);
    }catch(const web_not_found_exception& ex) {
        rsp.code = 404;
        rsp.write(ex.what());
    }
    //rsp.end();
    return rsp;
}

boost::filesystem::path web_file_server::get_file(const std::string& file)
{
    boost::filesystem::path base_dir(path);
    boost::filesystem::path file_to_read = base_dir / file;    
    LOG4CXX_INFO(logger, "serving file: "<<file_to_read.string());
    boost::filesystem::file_status status = boost::filesystem::status(file_to_read);
    if(status.type() != boost::filesystem::regular_file)
    {        
        throw web_not_found_exception("Cannot find "+file);
    }
    else
    {
        return file_to_read;
    }    
}
std::string web_file_server::get_mime_type(const std::string& file)
{
  if(file.rfind(".css") == (file.length() - 4))
  {
    return "text/css";
  }
  if(file.rfind(".js") == (file.length() - 3))
  {
    return "application/javascript; charset=UTF-8";
  }
  if(file.rfind(".html") == (file.length() - 5))
  {
    return "text/html; charset=UTF-8";
  }
  
  return magic->get_mime(file);

}
