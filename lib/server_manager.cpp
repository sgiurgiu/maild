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

using namespace maild;
using boost::asio::ip::tcp;

log4cxx::LoggerPtr server_manager::logger(log4cxx::Logger::getLogger("server_manager"));

server_manager::server_manager(const server_options& options):options(options)
{
    start_cleanup_thread();
}
server_manager::server_manager()
{
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

void server_manager::start_cleanup_thread()
{
    if(cleanup_thread_created) return;
    cleanup_thread = std::thread([this](){
        try
        {
            pqxx::connection db(options.get_db_connection_string());
            db.prepare("delete_mail","delete from mails where date_received <= $1");
            while(!cleanup_done.load())
            {                
                std::chrono::system_clock::time_point now = std::chrono::system_clock::now () ;
                std::time_t obsolete_time_t = std::chrono::system_clock::to_time_t(now - std::chrono::seconds(options.get_keep_mail_seconds()));
                std::tm tm = *std::localtime(&obsolete_time_t);
                std::stringstream time_str;
                time_str<<std::put_time(&tm, "%F %T %z");
                pqxx::work w(db);
                pqxx::result result = w.prepared("delete_mail")
                    (time_str.str())
                    .exec();
                w.commit();
                if(!cleanup_done.load())
                {
                    LOG4CXX_INFO(logger, "Deleted "<<result.affected_rows()<<" mails older than "<<time_str.str());
                }
                std::unique_lock<std::mutex> lock(mu);
                stop_condition.wait_for(lock,std::chrono::seconds(options.get_check_mail_interval_seconds()));                
            }
        } 
        catch(const std::exception& ex)
        {
            if(!cleanup_done.load())
            {
                LOG4CXX_ERROR(logger, "Deleting rows encountered an exception: "<<ex.what());
            }            
        }
        catch(...)
        {
            if(!cleanup_done.load())
            {
                LOG4CXX_ERROR(logger, "Deleting rows encountered an unknown exception");
            }            
        }

    });
    cleanup_thread_created = true;
}

void server_manager::set_options(const server_options &options)
{
  this->options = options;
  start_cleanup_thread();
}

void server_manager::run()
{
  std::vector<std::unique_ptr<smtp_server>> servers;
  for(const auto& address : options.get_ips())
  {
    servers.push_back(std::make_unique<smtp_server>(io_service,address,options))  ;
  }
  io_service.run();
}

void server_manager::stop()
{
  LOG4CXX_INFO(logger, "Received stop command.");
  io_service.stop();
  cleanup_done.store(true);
}
