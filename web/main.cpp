#include "options.h"
#include "options_parser.h"

#include <web_options.h>
#include <web_server.h>

#include <iostream>
#include <fstream>
#include <filesystem>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logger.h>

using namespace maild;


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
    
    std::filesystem::path log_file(arguments.log_file);
    if(!std::filesystem::is_regular_file(log_file))
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
    
    std::filesystem::path conf_file(arguments.config_file);
    if(!std::filesystem::is_regular_file(conf_file))
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

    try
    {        
        LOG4CXX_INFO(logger, "Starting MailDWeb server...")                
        web_server server(options);
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
