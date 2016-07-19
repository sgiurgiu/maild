#include "web_server_impl.h"
#include "web_file_server.h"
#include "web_api_server.h"

using namespace maild;
log4cxx::LoggerPtr web_server_impl::logger(log4cxx::Logger::getLogger("web_server_impl"));
web_server_impl::web_server_impl()
{
}
web_server_impl::web_server_impl(const web_options& options):options(options)
{
}
void web_server_impl::set_options(const web_options& options)
{
    this->options = options;
}
void web_server_impl::run()
{
    app = std::make_unique<crow::SimpleApp>();
    web_file_server file_server(options.get_files_dir());
    LOG4CXX_DEBUG(logger, "DB connection string "<<options.get_db_connection_string());
    web_api_server api_server(options.get_db_connection_string());
    
    CROW_ROUTE((*app),"/api/mails/<string>")    
    .methods("GET"_method)
    ([&api_server](const std::string& username){
        return api_server.get_users_mails(username);
    });    
    
    CROW_ROUTE((*app),"/<string>")
    .methods("GET"_method)
    ([&file_server](const std::string& path){
        return file_server.get_file_contents(path);
    });
    CROW_ROUTE((*app),"/js/<string>")
    .methods("GET"_method)
    ([&file_server](const std::string& path){
        return file_server.get_file_contents("js/"+path);
    });
    CROW_ROUTE((*app),"/css/<string>")
    .methods("GET"_method)
    ([&file_server](const std::string& path){
        return file_server.get_file_contents("css/"+path);
    });
    
    CROW_ROUTE((*app),"/")
    .methods("GET"_method)
    ([&file_server]() {
        return file_server.get_file_contents("index.html");
    });
    
    for(const auto& ip : options.get_ips())
    {
        LOG4CXX_INFO(logger, "Binding to ip "<<ip);
        app->bindaddr(ip);
    }
    app->port(options.get_port());
    LOG4CXX_INFO(logger, "Listening on port "<<options.get_port());
    
    app->multithreaded().run();
}
void web_server_impl::stop()
{
    LOG4CXX_INFO(logger, "Received stop command.");
    app.release();
}
