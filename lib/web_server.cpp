#include "web_server.h"
#include "web_api_server.h"
#include "web_file_server.h"
#include "crow_all.h"

using namespace maild;
log4cxx::LoggerPtr web_server::logger(log4cxx::Logger::getLogger("web_server"));

web_server::web_server(const web_options& options):options(options)
{
}
void web_server::run()
{        
    crow::SimpleApp app;
    web_file_server file_server(options.get_files_dir());
    LOG4CXX_DEBUG(logger, "DB connection string "<<options.get_db_connection_string());
    web_api_server api_server(options.get_db_connection_string());
    
    CROW_ROUTE(app,"/api/mails/<int>/<string>")    
    .methods("GET"_method)
    ([&api_server](int id, const std::string& type){
        return api_server.get_mail(id,type);
    });    
    CROW_ROUTE(app,"/api/mails/<string>")    
    .methods("GET"_method)
    ([&api_server](const std::string& username){
        return api_server.get_users_mails(username);
    });    
    
     
    CROW_ROUTE(app,"/<path>")
    .methods("GET"_method)
    ([&file_server](const std::string& path){
        return file_server.get_file_contents(path);
    });    
        
    CROW_ROUTE(app,"/")
    .methods("GET"_method)
    ([&file_server]() {
        return file_server.get_file_contents("index.html");
    });
    
    for(const auto& ip : options.get_ips())
    {
        LOG4CXX_INFO(logger, "Binding to ip "<<ip);
        app.bindaddr(ip);
    }
    app.port(options.get_port());
    LOG4CXX_INFO(logger, "Listening on port "<<options.get_port());
    
    app.multithreaded().run();
    LOG4CXX_INFO(logger, "Received stop command.");
}
