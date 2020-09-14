#include "server_manager.h"
#include "smtp_server.h"
#include <pqxx/transaction>

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
    signals(io_context, SIGINT, SIGTERM),
    db(std::make_unique<pqxx::connection>(options.get_db_connection_string()))
{    
    prepare_database();
    start_cleanup_thread();
}

server_manager::~server_manager()
{
    cleanup_done.store(true);
    stop_condition.notify_all();
    if(cleanup_thread.joinable())
    {
        cleanup_thread.join();
    }
}

void server_manager::prepare_database()
{
    db->prepare("delete_mail","delete from mails where date_received <= NOW() - ('1 second'::interval * $1)");
    db->prepare("new_mail","insert into mails(from_address,to_address,body,date_received,username) values ($1,$2,$3,NOW(),$4)");
    //deadlock possible apparently. we're low load so we're fine
    db->prepare("mail_count_increment","update counters set counter=counter+1 where id='mails_received'");
    {
       pqxx::work w(*db);
       w.exec("create table if not exists counters (id varchar(50) primary key,counter bigint not null default 0)");
       w.exec("insert into counters (id, counter) values ('mails_received',0) on conflict do nothing");
       w.commit();
    }
}

void server_manager::start_cleanup_thread()
{
    if(cleanup_thread_created) return;
    cleanup_thread = std::thread([this](){
        try
        {

            while(!cleanup_done.load())
            {                
                pqxx::work w(*db);
                pqxx::result result = w.exec_prepared("delete_mail",options.get_keep_mail_seconds());
                w.commit();
                if(!cleanup_done.load() && result.affected_rows() > 0)
                {
                    spdlog::info("Deleted {} mails",result.affected_rows());
                }
                std::unique_lock<std::mutex> lock(mu);
                stop_condition.wait_for(lock,std::chrono::seconds(options.get_check_mail_interval_seconds()));                
            }
        } 
        catch(const std::exception& ex)
        {
            if(!cleanup_done.load())
            {
                spdlog::error("Deleting rows encountered an exception: {}",ex.what());
            }            
        }
        catch(...)
        {
            if(!cleanup_done.load())
            {
                spdlog::error("Deleting rows encountered an unknown exception");
            }            
        }

    });
    cleanup_thread_created = true;
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

  auto num_threads = std::thread::hardware_concurrency() - 1;
  num_threads = num_threads > 0 ? num_threads : 1;
  std::vector<std::thread> v;
  v.reserve(num_threads);
  for(size_t i =0;i<num_threads;i++) {
      v.emplace_back([this](){
          io_context.run();
      });
  }

  for(size_t i =0;i<num_threads;i++) {
      if(v.at(i).joinable()) {
        v.at(i).join();
      }
  }

}

void server_manager::stop()
{
  spdlog::info( "Received stop command.");
  cleanup_done.store(true);
  stop_condition.notify_all();
  io_context.stop();
}
