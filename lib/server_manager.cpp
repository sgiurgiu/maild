#include "server_manager.h"
#include "smtp_server.h"
#include <pqxx/transaction>
#include <pqxx/connection>

#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>

#include <spdlog/spdlog.h>

using namespace maild;
using boost::asio::ip::tcp;

server_manager::server_manager(const server_options& options):options(options),
    signals(io_context, SIGINT, SIGTERM),cleanup_timer(cleanup_context)
{    
    prepare_database();
}

server_manager::~server_manager()
{
}

void server_manager::prepare_database()
{
   pqxx::connection db(options.get_db_connection_string());
   pqxx::work w(db);
   w.exec("create table if not exists counters (id varchar(50) primary key,counter bigint not null default 0)");
   w.exec("insert into counters (id, counter) values ('mails_received',0) on conflict do nothing");
   w.commit();
}

void server_manager::cleanup_messages(const boost::system::error_code& error)
{
    if(error) return;
    try
    {
        pqxx::connection db(options.get_db_connection_string());
        db.prepare("delete_mail","delete from mails where date_received <= NOW() - ('1 second'::interval * $1)");
        pqxx::work w(db);
        pqxx::result result = w.exec_prepared("delete_mail",options.get_keep_mail_seconds());
        w.commit();
        if(result.affected_rows() > 0)
        {
            spdlog::info("Deleted {} mails",result.affected_rows());
        }
    }
    catch(const std::exception& ex)
    {
        spdlog::error("Deleting rows encountered an exception: {}",ex.what());
    }
    catch(...)
    {
        spdlog::error("Deleting rows encountered an unknown exception");
    }
    cleanup_timer.expires_after(std::chrono::seconds(options.get_check_mail_interval_seconds()));
    cleanup_timer.async_wait(std::bind(&server_manager::cleanup_messages,this,std::placeholders::_1));
}

void server_manager::run()
{
  std::vector<std::unique_ptr<smtp_server>> servers;  
  for(const auto& server_info : options.get_servers())
  {
      if(!server_info.enabled) continue;
      auto server = std::make_unique<smtp_server>(io_context,options.get_db_connection_string(),
                                                  server_info,options.get_domain_name(),
                                                  options.get_certificates());
      server->run();
      servers.push_back(std::move(server));
  }
  signals.async_wait(
    [this](const boost::system::error_code& /*error*/, int /*signal_number*/){
        stop();
    });

  cleanup_timer.expires_after(std::chrono::seconds(options.get_check_mail_interval_seconds()));
  cleanup_timer.async_wait(std::bind(&server_manager::cleanup_messages,this,std::placeholders::_1));

  auto num_threads = std::thread::hardware_concurrency();
  num_threads = std::max(num_threads,(decltype (num_threads))servers.size());
  std::vector<std::thread> v;
  v.reserve(num_threads+1);
  using run_function = boost::asio::io_context::count_type(boost::asio::io_service::*)();
  for(size_t i =0;i<num_threads;i++) {
      v.emplace_back(std::thread(std::bind(
                                     static_cast<run_function>(&boost::asio::io_context::run)
                                     ,&io_context)));
  }
  v.emplace_back(std::thread(std::bind(
                                 static_cast<run_function>(&boost::asio::io_context::run)
                                 ,&cleanup_context)));
  for(size_t i=0;i<num_threads+1;i++) {
      if(v.at(i).joinable()) {
        v.at(i).join();
      }
  }
}

void server_manager::stop()
{
  spdlog::info( "Received stop command.");
  cleanup_context.stop();
  io_context.stop();
}
