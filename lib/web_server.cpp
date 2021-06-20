#include "web_server.h"
#include "web_api_server.h"
#include "web_file_server.h"

#include <vector>
#include <thread>

#include <http/reactor/listener.hxx>
#include <http/reactor/session.hxx>
#include <http/reactor/ssl/session.hxx>

#include <http/basic_router.hxx>
#include <http/out.hxx>
#include <http/param.hxx>
#include <http/literals.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/buffers_range.hpp>

#include <spdlog/spdlog.h>

using namespace maild;
namespace beast = boost::beast;


web_server::web_server(const web_options& options):options(options),
    ssl_context(boost::asio::ssl::context::tls_server)
{
}
void web_server::run()
{        
    using namespace _0xdead4ead;

    using ssl_http_session = http::reactor::ssl::_default::session_type;
    using http_session = http::reactor::_default::session_type;
    using http_listener = http::reactor::_default::listener_type;

    if(options.is_ssl())
    {
        auto cert = options.get_web_certificate();
        ssl_context.set_options(boost::asio::ssl::context::default_workarounds
                                | boost::asio::ssl::context::no_sslv2
                                | boost::asio::ssl::context::single_dh_use);
        ssl_context.set_default_verify_paths();
        ssl_context.use_certificate_chain_file(cert.certificate_chain);
        ssl_context.use_private_key_file(cert.private_key,boost::asio::ssl::context::pem);
        ssl_context.use_tmp_dh_file(cert.dh_file);
    }
    boost::asio::io_context ioc;

    web_file_server file_server(options.get_files_dir());
    spdlog::debug("DB connection string {}",options.get_db_connection_string());

    // Error and warning handler
    const auto& onError = [&ioc](auto system_error_code, auto from) {

        spdlog::error("From: {}, Info: {}",std::string(from),system_error_code.message());

        // I/O context will be stopped, if code value is EADDRINUSE or EACCES
        if (system_error_code == boost::system::errc::address_in_use or
                system_error_code == boost::system::errc::permission_denied)
            ioc.stop();
    };

    using normal_router = http::basic_router<http_session>;
    using ssl_router = http::basic_router<ssl_http_session>;
    auto const address = boost::asio::ip::address::from_string(*options.get_ips().begin());
    auto const port = static_cast<unsigned short>(options.get_port());

    if(options.is_ssl())
    {
        ssl_router router{std::regex::ECMAScript};
        fill_router_routes<ssl_router>(router,file_server);

        // Handler incoming connections
        const auto& onAccept = [&](auto asio_socket) {
            auto endpoint = asio_socket.remote_endpoint();
            spdlog::debug("{}:{} connected!",endpoint.address().to_string(),endpoint.port());

            ssl_http_session::handshake(ssl_context, std::move(asio_socket), router,
                                        [](auto context) {
                                             context.recv();
                                        }, onError);
        };
        spdlog::debug("Start accepting SSL connections on {}:{}",address.to_string(),port);

        // Start accepting
        http_listener::launch(ioc, {address, port}, onAccept, onError);
        start_lisening(ioc);
    }
    else
    {
        normal_router router{std::regex::ECMAScript};
        fill_router_routes<normal_router>(router,file_server);
        // Handler incoming connections
        const auto& onAccept = [&](auto asio_socket) {
            auto endpoint = asio_socket.remote_endpoint();
            spdlog::debug("{}:{} connected!",endpoint.address().to_string(),endpoint.port());

            // Start receive HTTP request
            http_session::recv(std::move(asio_socket), router, onError);
        };
        spdlog::debug("Start accepting on {}:{}",address.to_string(),port);

        // Start accepting
        http_listener::launch(ioc, {address, port}, onAccept, onError);
        start_lisening(ioc);
    }
}

void web_server::start_lisening(boost::asio::io_context& ioc)
{
    boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);
    // Capture SIGINT and SIGTERM to perform a clean shutdown
    sig_set.async_wait([&](boost::system::error_code const&, int sig) {
        spdlog::info("Capture {}. Stop!",sig == SIGINT ? "SIGINT" : "SIGTERM");
        ioc.stop();
    });

    uint32_t pool_size = 1;//std::thread::hardware_concurrency();

   // Run the I/O service on the requested number of threads
   std::vector<std::thread> threads;
   threads.reserve(pool_size > 0 ? pool_size : 2);
   for(uint32_t i = 0; i < pool_size; i++)
       threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                      (&boost::asio::io_context::run), std::ref(ioc)));

   // Block until all the threads exit
   for(auto& t : threads)
       t.join();
}

template <typename Router>
void web_server::fill_router_routes(Router& router, web_file_server &file_server)
{
    using namespace _0xdead4ead;
    // Set router targets
    using pack2 = http::param::pack<int,  std::string>;
    router.template param<pack2>().get(apiMailContentRegex,
                              [this](auto request, auto context, auto args) {
        web_api_server api_server(options.get_db_connection_string());
        auto id = std::get<0>(args);
        auto type = std::get<1>(args);
        spdlog::debug("Retrieving mail for id {} and type {} for url: {}",id,type,request.target().to_string());
        auto rsp = api_server.get_mail(request,id,type);
        context.send(rsp);
    });
    using pack3 = http::param::pack<std::string>;
    router.template param<pack3>().get(apiMailsRegex,
                              [this](auto request, auto context, auto args) {
        web_api_server api_server(options.get_db_connection_string());
        spdlog::debug("Retrieving all mails for id {} for url: {}",std::get<0>(args),request.target().to_string());
        context.send(api_server.get_users_mails(request,std::get<0>(args)));
    });
    router.get(indexHtmlRegex, [&file_server](auto request, auto context) {
        context.send(file_server.get_file_contents(request,"index.html"));
    });
    router.get(filesRegex, [&file_server](const auto& request, auto context) {
        context.send(file_server.get_file_contents(request,request.target().to_string()));
    });
    router.all(everythingElseRegex, [](auto request, auto context) {
        boost::beast::http::response<boost::beast::http::string_body> rsp;
        rsp.result(boost::beast::http::status::not_found);
        rsp.version(request.version());
        rsp.set(boost::beast::http::field::server, MAILD_STRING);
        spdlog::info(" Resource not found {}",request.target().to_string());
        std::string contents = "Resource not found:"+request.target().to_string()+"\n";
        rsp.set(boost::beast::http::field::content_length, std::to_string(contents.length()));
        rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
        rsp.body() = contents;
        rsp.prepare_payload();
        rsp.keep_alive(request.keep_alive());
        context.send(rsp);
    });
}
