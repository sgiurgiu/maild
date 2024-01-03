#include "server_options.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include "toml.hpp"
#include "utils.h"

using namespace maild;

server_options::server_options()
{

}
void server_options::load(std::istream& conf_stream)
{
    toml::parse_result configuration;
    try
    {
        configuration = toml::parse(conf_stream);
    }
    catch(const toml::parse_error& er)
    {
      throw;
    }

    if(configuration.empty())
    {
      throw std::runtime_error("Invalid configuration file");
    }
    utils::configure_logs(configuration["log"]);

    auto main_conf = configuration["main"];
    db_connection_string = main_conf["database_url"].value_or(db_connection_string);
    spdlog::debug("db_connection_string: {}",db_connection_string);
    domain_name = main_conf["domain"].value_or(domain_name);
    spdlog::debug( "domain_name: {}",domain_name);
    keep_mail_seconds = main_conf["keep_mail"].value_or(keep_mail_seconds);
    spdlog::debug("keep_mail_seconds:",keep_mail_seconds);
    check_mail_interval_seconds = main_conf["check_mail_interval"].value_or(check_mail_interval_seconds);
    spdlog::debug("check_mail_interval_seconds: {} ",check_mail_interval_seconds);
    auto certs = configuration["certificates"];
    certificate_files.certificate_chain=certs["certificate_chain"].value_or("");
    certificate_files.private_key=certs["private_key"].value_or("");
    certificate_files.dh_file=certs["dh_file"].value_or("");

    auto servers_conf = configuration["servers"];
    for(const auto& server_conf : *servers_conf.as_table())
    {
        auto& value = *(server_conf.second.as_table());
        server a_server;
        a_server.ip = value["ip"].value_or(a_server.ip);
        a_server.port = value["port"].value_or(a_server.port);
        a_server.enabled = value["enabled"].value_or(a_server.enabled);
        a_server.ssl = value["ssl"].value_or(a_server.ssl);
        servers.push_back(a_server);
    }

}

certificates server_options::get_certificates() const
{
    return certificate_files;
}
std::string server_options::get_db_connection_string() const
{
  return db_connection_string;
}

std::string server_options::get_domain_name() const
{
  return domain_name;
}

std::vector<server> server_options::get_servers() const
{
  return servers;
}

int64_t server_options::get_keep_mail_seconds() const
{
    return keep_mail_seconds;
}

int64_t server_options::get_check_mail_interval_seconds() const
{
    return check_mail_interval_seconds;
}
