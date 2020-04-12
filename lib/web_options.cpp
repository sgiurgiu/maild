#include "web_options.h"
#include "json.hpp"
#include <stdexcept>
#include <log4cxx/logger.h>

using namespace maild;
using json = nlohmann::json;

web_options::web_options()
{

}
void web_options::load(std::istream& conf_stream)
{
  log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("web_options"));
  LOG4CXX_DEBUG(logger, "Parsing web options");
  auto conf_value = json::parse(conf_stream);
  if(!conf_value.is_object())
  {
      throw std::runtime_error("Invalid configuration file");
  }
  db_connection_string = conf_value["database_url"].get<std::string>();
  LOG4CXX_DEBUG(logger, "db_connection_string:"<<db_connection_string);
  domain_name = conf_value["domain"].get<std::string>();
  LOG4CXX_DEBUG(logger, "domain_name:"<<domain_name);
  files_dir = conf_value["files_dir"].get<std::string>();
  LOG4CXX_DEBUG(logger, "files_dir:"<<files_dir);
  api_prefix = conf_value["api_prefix"].get<std::string>();
  LOG4CXX_DEBUG(logger, "api_prefix:"<<api_prefix);
  port = conf_value["port"].get<int>();
  LOG4CXX_DEBUG(logger, "port:"<<port);
  auto ips_value = conf_value["ips"];
  if(ips_value.is_array())
  {
      ips.clear();
      for(const auto& ip_value : ips_value)
      {          
          auto inserted = ips.insert(ip_value.get<std::string>());
          LOG4CXX_DEBUG(logger, "ips:"<<(*inserted.first));
      }
  }
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
