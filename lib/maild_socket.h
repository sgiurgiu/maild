#ifndef MAILD_SOCKET_H
#define MAILD_SOCKET_H

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/read_until.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include "server_options.h"

namespace maild {

class maild_socket
{
public:
    maild_socket(boost::asio::strand<boost::asio::io_context::executor_type>& strand,
                 const certificates& certificate_files):
        sock(strand),ssl_context(boost::asio::ssl::context::sslv23_server)
    {
        ssl_context.set_options(boost::asio::ssl::context::default_workarounds
                                | boost::asio::ssl::context::single_dh_use);
        ssl_context.set_default_verify_paths();
        ssl_context.use_certificate_chain_file(certificate_files.certificate_chain);
        ssl_context.use_private_key_file(certificate_files.private_key,boost::asio::ssl::context::pem);
        ssl_context.use_tmp_dh_file(certificate_files.dh_file);
    }
    boost::asio::ip::tcp::socket& get_socket()
    {
        if(ssl && stream)
        {
            return stream->next_layer();
        }
        else
        {
            return sock;
        }
    }
    void close()
    {
        boost::system::error_code ignored_ec;
        if(ssl && stream)
        {
            stream->shutdown(ignored_ec);
            stream.reset();
        }
        sock.close(ignored_ec);
    }
    void set_ssl(bool ssl_flag)
    {
        ssl = ssl_flag;
        if(ssl)
        {
            stream = std::make_unique<ssl_stream>(sock,ssl_context);
            boost::system::error_code ec;
            spdlog::debug("Preparing for ssl handshake");
            stream->handshake(boost::asio::ssl::stream_base::server, ec);
            if(ec.failed())
            {
                spdlog::error("SSL Handshake failed: {}",ec.message());
                ssl = false;
                stream.reset();
                sock.close(ec);
            }
            else
            {
                spdlog::info("ssl handshake succeeded");
            }
        }
        else
        {
            if(stream)
            {
                stream->shutdown();
                stream.reset();
            }            
        }
        spdlog::default_logger()->flush();
    }
    bool is_open() const
    {
        return sock.is_open();
    }
    template<typename Request,typename Callback>
    void write(Request& request,Callback callback)
    {
        if(ssl && stream)
        {
            boost::asio::async_write(*stream,request,callback);
        }
        else
        {
            boost::asio::async_write(sock,request,callback);
        }
    }
    template<typename Response,typename Callback>
    void read_until(Response& response, const std::string& delimiter,Callback callback)
    {
        if(ssl && stream)
        {
            boost::asio::async_read_until(*stream,response,delimiter,callback);
        }
        else
        {
            boost::asio::async_read_until(sock,response,delimiter,callback);
        }
    }
private:
    using ssl_stream = boost::asio::ssl::stream<boost::asio::ip::tcp::socket&>;
    boost::asio::ip::tcp::socket sock;
    boost::asio::ssl::context ssl_context;
    std::unique_ptr<ssl_stream> stream;
    bool ssl = false;
};

} // namespace maild

#endif // MAILD_SOCKET_H
