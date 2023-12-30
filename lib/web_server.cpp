#include "web_server.h"
#include "web_api_server.h"
#include "web_file_server.h"

#include <vector>
#include <thread>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>

#include <crow.h>

#include <spdlog/spdlog.h>

using namespace maild;
namespace beast = boost::beast;


web_server::web_server(const web_options& options):options(options),
    ssl_context(boost::asio::ssl::context::tls_server)
{
}
void web_server::run()
{
    crow::SimpleApp app;

    web_file_server file_server(options.get_files_dir());
    spdlog::debug("DB connection string {}",options.get_db_connection_string());

    CROW_ROUTE(app, "/api/mail/<int>/<string>").methods("GET"_method)([con = options.get_db_connection_string()](int id, std::string type)
                                                                      {   
        web_api_server api_server(con);
        auto response = api_server.get_mail(id,type);
        crow::response resp;
        resp.code = response.result_int();
        resp.body = response.body();
        for(const auto& h : response)
        {
            resp.add_header(h.name_string(), h.value());
        }
        return resp; 
    });

    CROW_ROUTE(app, "/api/mails/<string>").methods("GET"_method)([con = options.get_db_connection_string()](const std::string &username)
                                                                 {   
        web_api_server api_server(con);
        auto response = api_server.get_users_mails(username);
        crow::response resp;
        resp.code = response.result_int();
        resp.body = response.body();
        for(const auto& h : response)
        {
            resp.add_header(h.name_string(), h.value());
        }
        return resp; 
    });

    CROW_ROUTE(app, "/<path>").methods("GET"_method)([&file_server](const std::string &path)
    {   
        auto response = file_server.get_file_contents(path);
        crow::response resp;
        resp.code = response.result_int();
        resp.body = response.body();
        for(const auto& h : response)
        {
            resp.add_header(h.name_string(), h.value());
        }
        return resp; 
    });

    CROW_ROUTE(app, "/").methods("GET"_method)([&file_server]()
    {   
        auto response = file_server.get_file_contents("index.html");
        crow::response resp;
        resp.code = response.result_int();
        resp.body = response.body();
        for(const auto& h : response)
        {
            resp.add_header(h.name_string(), h.value());
        }
        return resp; 
    });
    for (const auto &ip : options.get_ips())
    {
        spdlog::info("Binding to ip {}", ip);
        app.bindaddr(ip);
    }
    app.port(options.get_port());

    app.multithreaded().run();
}

