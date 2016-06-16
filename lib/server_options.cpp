#include "server_options.h"
#define PICOJSON_USE_INT64
#include "picojson.h"

#include <stdexcept>

using namespace maild;

server_options::server_options()
{

}
void server_options::load(std::istream& conf_stream)
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
  keep_mail_seconds = obj.at("keep_mail").get<int64_t>();
  check_mail_interval_seconds = obj.at("check_mail_interval").get<int64_t>();
  auto plain_it = obj.find("plain");
  if(plain_it != obj.end())
  {
      plain_enabled = plain_it->second.get("enabled").evaluate_as_boolean();
      if(plain_enabled)
      {
          plain_port = plain_it->second.get("port").get<int64_t>();
      }
  }

  auto secure_it = obj.find("secure");
  if(secure_it != obj.end())
  {
      secure_enabled = secure_it->second.get("enabled").evaluate_as_boolean();
      if(secure_enabled)
      {
          secure_port = secure_it->second.get("port").get<int64_t>();
      }
  }
  picojson::value ips_value = conf_value.get("ips");
  if(ips_value.is<picojson::array>())
  {
      for(const auto& ip_value : ips_value.get<picojson::array>())
      {
          ips.insert(ip_value.get<std::string>());
      }
  }
}

int server_options::get_plain_port() const
{
    return plain_port;
}

std::string server_options::get_db_connection_string() const
{
  return db_connection_string;
}

std::string server_options::get_domain_name() const
{
  return domain_name;
}

std::set<std::string> server_options::get_ips() const
{
    return ips;
}

int64_t server_options::get_keep_mail_seconds() const
{
    return keep_mail_seconds;
}

int64_t server_options::get_check_mail_interval_seconds() const
{
    return check_mail_interval_seconds;
}
