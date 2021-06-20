#include "web_options.h"
#include <stdexcept>
#include <spdlog/spdlog.h>
#include "toml.hpp"
#include "utils.h"

using namespace maild;

web_options::web_options()
{

}
void web_options::load(std::istream& conf_stream)
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
    files_dir = main_conf["files_dir"].value_or(files_dir);
    spdlog::debug( "files_dir:{}",files_dir);
    api_prefix = main_conf["api_prefix"].value_or(api_prefix);
    spdlog::debug( "api_prefix:{}",api_prefix);
    port = main_conf["port"].value_or(8080);
    ssl = main_conf["ssl"].value_or(false);

    spdlog::debug( "port:{}",port);
    auto ips_value = main_conf["ips"];
    if(ips_value.is_array())
    {
      ips.clear();
      for(const auto& ip_value : *ips_value.as_array())
      {
          auto inserted = ips.insert(ip_value.value_or<std::string>("127.0.0.1"));
          spdlog::debug( "ips:{}",(*inserted.first));
      }
    }
    if(configuration.contains("certificate"))
    {
        auto certs = configuration["certificate"];
        certificate.certificate_chain=certs["certificate_chain"].value_or("");
        certificate.private_key=certs["private_key"].value_or("");
        certificate.dh_file=certs["dh_file"].value_or("");
    }
}
web_certificate web_options::get_web_certificate() const
{
    return certificate;
}
bool web_options::is_ssl() const
{
    return ssl;
}

int web_options::get_port() const
{
    return port;
}

std::string web_options::get_db_connection_string() const
{
  return db_connection_string;
}

std::string web_options::get_domain_name() const
{
  return domain_name;
}

std::set<std::string> web_options::get_ips() const
{
    return ips;
}
std::string web_options::get_api_prefix() const
{
    return api_prefix;
}
std::string web_options::get_files_dir() const
{
    return files_dir;
}
