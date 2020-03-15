#include "web_server.h"
#include "web_api_server.h"
#include "web_file_server.h"

#include <vector>
#include <thread>

#include <http/reactor/listener.hxx>
#include <http/reactor/session.hxx>

#include <http/basic_router.hxx>
#include <http/out.hxx>
#include <http/param.hxx>
#include <http/literals.hxx>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <boost/beast/core/buffers_range.hpp>

using namespace maild;
namespace beast = boost::beast;

log4cxx::LoggerPtr web_server::logger(log4cxx::Logger::getLogger("web_server"));

web_server::web_server(const web_options& options):options(options)
{
}
void web_server::run()
{        
    using namespace _0xdead4ead;

    using http_session = http::reactor::_default::session_type;
    using http_listener = http::reactor::_default::listener_type;

    boost::asio::io_context ioc;
    boost::asio::posix::stream_descriptor out{ioc, ::dup(STDERR_FILENO)};
    boost::asio::signal_set sig_set(ioc, SIGINT, SIGTERM);

    web_file_server file_server(options.get_files_dir());
    LOG4CXX_DEBUG(logger, "DB connection string "<<options.get_db_connection_string());
    

    http::basic_router<http_session> router{std::regex::ECMAScript};

    // Set router targets
    using pack2 = http::param::pack<int,  std::string>;
    router.param<pack2>().get("/api/mails/(\\d+)/(\\w+)",
                              [this](const auto& request, auto context, auto args) {
        web_api_server api_server(options.get_db_connection_string());
        context.send(api_server.get_mail(request,std::get<0>(args),std::get<1>(args)));
    });
    using pack3 = http::param::pack<std::string>;
    router.param<pack3>().get("/api/mails/(\\w+)",
                              [this](const auto& request, auto context, auto args) {
        web_api_server api_server(options.get_db_connection_string());
        context.send(api_server.get_users_mails(request,std::get<0>(args)));
    });
    router.get(R"(^/$)", [&file_server](auto request, auto context) {
        context.send(file_server.get_file_contents(request,"index.html"));
    });
    router.get(R"(^/(?:(?!api/mails)[a-zA-Z_\s\-\.\/])+$)", [&file_server](const auto& request, auto context) {
        context.send(file_server.get_file_contents(request,request.target().to_string()));
    });
    router.all(R"(^.*$)", [](auto request, auto context) {
        boost::beast::http::response<boost::beast::http::string_body> rsp;
        rsp.result(boost::beast::http::status::not_found);
        rsp.version(request.version());
        rsp.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
        LOG4CXX_INFO(logger, " Resource not found "<<request.target().to_string());
        std::string contents = "Resource not found:"+request.target().to_string()+"\n";
        rsp.set(boost::beast::http::field::content_length, std::to_string(contents.length()));
        rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
        rsp.body() = contents;
        rsp.prepare_payload();
        rsp.keep_alive(request.keep_alive());
        context.send(rsp);
    });

    // Error and warning handler
    const auto& onError = [&ioc,&out](auto system_error_code, auto from) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "From:", from, "Info:", system_error_code.message());

        // I/O context will be stopped, if code value is EADDRINUSE or EACCES
        if (system_error_code == boost::system::errc::address_in_use or
                system_error_code == boost::system::errc::permission_denied)
            ioc.stop();
    };

    // Handler incoming connections
    const auto& onAccept = [&](auto asio_socket) {
        auto endpoint = asio_socket.remote_endpoint();

        http::out::prefix::version::time::pushn<std::ostream>(
                    out, endpoint.address().to_string() + ':' + std::to_string(endpoint.port()), "connected!");

        // Start receive HTTP request
        http_session::recv(std::move(asio_socket), router, onError);
    };

    auto const address = boost::asio::ip::address_v4::any();
    auto const port = static_cast<unsigned short>(8080);

    http::out::prefix::version::time::pushn<std::ostream>(
                out, "Start accepting on", address.to_string() + ':' + std::to_string(port));

    // Start accepting
    http_listener::launch(ioc, {address, port}, onAccept, onError);

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    sig_set.async_wait([&](boost::system::error_code const&, int sig) {
        http::out::prefix::version::time::pushn<std::ostream>(
                    out, "Capture", sig == SIGINT ? "SIGINT." : "SIGTERM.", "Stop!");
        ioc.stop();
    });

    uint32_t pool_size = std::thread::hardware_concurrency() * 2;

   // Run the I/O service on the requested number of threads
   std::vector<std::thread> threads;
   threads.reserve(pool_size > 0 ? pool_size : 4);
   for(uint32_t i = 0; i < pool_size; i++)
       threads.emplace_back(std::bind(static_cast<std::size_t (boost::asio::io_context::*)()>
                                      (&boost::asio::io_context::run), std::ref(ioc)));

   // Block until all the threads exit
   for(auto& t : threads)
       t.join();
}
