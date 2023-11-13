#include "web_file_server.h"
#include "web_not_found_exception.h"
#include "magic_handler.h"
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

using namespace maild;

web_file_server::web_file_server(const std::string& path):path(path),magic(std::make_unique<magic_handler>())
{
}
web_file_server::~web_file_server() = default;

boost::beast::http::response<boost::beast::http::string_body>
    web_file_server::get_file_contents(const std::string& file)
{    
    boost::beast::http::response<boost::beast::http::string_body> rsp;
    try{                
        std::string file_name = get_file(file).string();
        std::ifstream in (file_name);   
        rsp.result(boost::beast::http::status::ok);
        rsp.set(boost::beast::http::field::server, MAILD_STRING);
        std::string contents = (static_cast<std::stringstream const&>(std::stringstream() << in.rdbuf()).str());
        rsp.set(boost::beast::http::field::content_length, std::to_string(contents.length()));
        std::string mime = get_mime_type(file_name);
        spdlog::debug("file: {} has mime {}",file,mime);
        rsp.set(boost::beast::http::field::content_type,mime);
        rsp.body() = contents;
    }
    catch(const web_not_found_exception& ex)
    {
        rsp.result(boost::beast::http::status::not_found);
        rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
        rsp.body() = ex.what();
    }
    rsp.prepare_payload();
    return rsp;
}

std::filesystem::path web_file_server::get_file(const std::string& file)
{
    std::filesystem::path base_dir(path);
    auto file_to_serve = file;
    //if(!file_to_serve.empty() && file_to_serve[0]=='/') file_to_serve = file_to_serve.substr(1);
    std::filesystem::path file_(file);
    if(file_.is_absolute()) file_ = file_.relative_path();
    std::filesystem::path file_to_read = base_dir / file_;
    spdlog::info( "serving file: {}",file_to_read.string());
    std::filesystem::file_status status = std::filesystem::status(file_to_read);
    if(status.type() != std::filesystem::file_type::regular)
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
