#include "options.h"
#include "options_parser.h"

#include <web_options.h>
#include <web_server.h>

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logger.h>

#include <unistd.h>
#include <signal.h>

using namespace maild;
web_server server;
void handle_signal(int /*sig*/)
{
    server.stop();
}

int main(int argc, char **argv) {
    options_parser parser;
    if(argc==1) {
        parser.show_help(std::cout);
        return 1;
    }

    options arguments = parser.parse_arguments(argc,argv);

    if(arguments.help) {
        parser.show_help(std::cout);
        return 1;
    }

    if(arguments.version) {
        parser.show_help(std::cout);
        return 1;
    }
    
    boost::filesystem::path log_file(arguments.log_file);
    boost::filesystem::file_status status = boost::filesystem::status(log_file);
    if(status.type() != boost::filesystem::regular_file)
    {
        std::cout << arguments.log_file << " cannot be found, using default logging settings"<< std::endl;
        log4cxx::BasicConfigurator::configure();        
    }
    else
    {
        log4cxx::PropertyConfigurator::configure(arguments.log_file);
    }
    log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());
    if(arguments.verbose)
    {        
        logger->setLevel(log4cxx::Level::getDebug());
    }
    if(arguments.quiet)
    {
        logger->setLevel(log4cxx::Level::getFatal());
    }
    
    boost::filesystem::path conf_file(arguments.config_file);
    status = boost::filesystem::status(conf_file);
    if(status.type() != boost::filesystem::regular_file)
    {
        std::cout << arguments.config_file << " is not a regular file"<< std::endl;
        return 1;
    }
    
    web_options options;
    try
    {
        std::ifstream conf_file_stream(arguments.config_file);
        options.load(conf_file_stream);
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    
    if(arguments.daemon)
    {
        if(daemon(0,0))
        {
            LOG4CXX_FATAL(logger, "Cannot run in the background");
            return 1;
        }
    }

    signal (SIGQUIT, handle_signal);
    signal (SIGABRT, handle_signal);
    signal (SIGINT, handle_signal);
    signal (SIGTERM, handle_signal);

    try
    {
        LOG4CXX_INFO(logger, "Starting MailDWeb server...")                
        server.run();
        LOG4CXX_INFO(logger, "Stopping MailDWeb server...")
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        LOG4CXX_ERROR(logger, "Error occurred, shutting down. Cause: "<<ex.what())
        return 1;
    }
    catch(...)
    {
      LOG4CXX_ERROR(logger, "Error occurred, shutting down.")
      return 1;
    }
    
    return 0;
}