#ifndef MAILD_SERVER_MANAGER_H
#define MAILD_SERVER_MANAGER_H

#include "server_options.h"
#include <boost/asio/io_service.hpp>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <log4cxx/logger.h>

namespace maild {

class server_manager
{
public:
    server_manager(const server_options& options);
    server_manager();
    ~server_manager();
    void set_options(const server_options& options);
    void run();
    void stop();
private:
    void start_cleanup_thread();
private:
    std::atomic_bool cleanup_done{false};
    bool cleanup_thread_created = false;
    std::thread cleanup_thread;
    std::mutex mu;
    std::condition_variable stop_condition;
    server_options options;    
    boost::asio::io_service io_service;    
    static log4cxx::LoggerPtr logger;
};

}

#endif // MAILD_SERVER_MANAGER_H
