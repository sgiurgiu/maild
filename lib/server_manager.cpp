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

server_manager::server_manager(const server_options& options):options(options),signals(io_service, SIGINT, SIGTERM)
{    
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

void server_manager::start_cleanup_thread()
{
    if(cleanup_thread_created) return;
    cleanup_thread = std::thread([this](){
        try
        {
            pqxx::connection db(options.get_db_connection_string());
            std::string sql = "delete from mails where date_received <= NOW() - interval '";
            sql+= std::to_string(options.get_keep_mail_seconds());
            sql+=" seconds'";
            LOG4CXX_DEBUG(logger, "Deletion sql "<<sql);
            db.prepare("delete_mail",sql);
            while(!cleanup_done.load())
            {                
                pqxx::work w(db);
                pqxx::result result = w.exec_prepared("delete_mail");
                w.commit();
                if(!cleanup_done.load() && result.affected_rows() > 0)
                {
                    LOG4CXX_INFO(logger, "Deleted "<<result.affected_rows()<<" mails");
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

void server_manager::run()
{
  std::vector<std::unique_ptr<smtp_server>> servers;
  for(const auto& address : options.get_ips())
  {
    servers.push_back(std::make_unique<smtp_server>(io_service,address,options))  ;
  }
  signals.async_wait(
    [this](const boost::system::error_code& /*error*/, int /*signal_number*/){
        stop();
    });
  
  io_service.run();
}

void server_manager::stop()
{
  LOG4CXX_INFO(logger, "Received stop command.");
  cleanup_done.store(true);
  io_service.stop();  
}
