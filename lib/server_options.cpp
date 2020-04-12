#include "server_options.h"
#include "json.hpp"
#include <stdexcept>
#include <log4cxx/logger.h>

using namespace maild;
using json = nlohmann::json;

server_options::server_options()
{

}
void server_options::load(std::istream& conf_stream)
{
  log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("server_options"));
  LOG4CXX_DEBUG(logger, "Parsing server options");
  auto conf_value = json::parse(conf_stream);
  if(!conf_value.is_object())
  {
      throw std::runtime_error("Invalid configuration file");
  }
  db_connection_string = conf_value["database_url"].get<std::string>();
  LOG4CXX_DEBUG(logger, "db_connection_string:"<<db_connection_string);
  domain_name = conf_value["domain"].get<std::string>();
  LOG4CXX_DEBUG(logger, "domain_name:"<<domain_name);
  keep_mail_seconds = conf_value["keep_mail"].get<int64_t>();
  LOG4CXX_DEBUG(logger, "keep_mail_seconds:"<<keep_mail_seconds);
  check_mail_interval_seconds = conf_value["check_mail_interval"].get<int64_t>();
  LOG4CXX_DEBUG(logger, "check_mail_interval_seconds:"<<check_mail_interval_seconds);
  auto plain_it = conf_value.find("plain");
  if(plain_it != conf_value.end())
  {
      plain_enabled = plain_it->at("enabled").get<bool>();
      LOG4CXX_DEBUG(logger, "plain_enabled:"<<plain_enabled);
      if(plain_enabled)
      {
          plain_port = plain_it->at("port").get<int64_t>();
          LOG4CXX_DEBUG(logger, "plain_port:"<<plain_port);
      }
  }

  auto secure_it = conf_value.find("secure");
  if(secure_it != conf_value.end())
  {
      secure_enabled = secure_it->at("enabled").get<bool>();
      LOG4CXX_DEBUG(logger, "secure_enabled:"<<secure_enabled);
      if(secure_enabled)
      {
          secure_port = secure_it->at("port").get<int64_t>();
          LOG4CXX_DEBUG(logger, "secure_port:"<<secure_port);
      }
  }
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
