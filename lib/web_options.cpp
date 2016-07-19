#include "web_options.h"
#define PICOJSON_USE_INT64
#include "picojson.h"

#include <stdexcept>

using namespace maild;

web_options::web_options()
{

}
void web_options::load(std::istream& conf_stream)
{
  picojson::value conf_value;
  picojson::parse(conf_value,conf_stream);
  if(!conf_value.is<picojson::object>())
  {
      throw std::runtime_error("Invalid configuration file");
  }
  const picojson::object& obj = conf_value.get<picojson::object>();
  db_connection_string = obj.at("database_url").get<std::string>();
  domain_name = obj.at("domain").get<std::string>();
  files_dir = obj.at("files_dir").get<std::string>();
  api_prefix = obj.at("api_prefix").get<std::string>();
  port = obj.at("port").get<int64_t>();
  picojson::value ips_value = conf_value.get("ips");
  if(ips_value.is<picojson::array>())
  {
      ips.clear();
      for(const auto& ip_value : ips_value.get<picojson::array>())
      {          
          ips.insert(ip_value.get<std::string>());
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
