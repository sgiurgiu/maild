#include "options.h"
#include "options_parser.h"

#include <web_options.h>
#include <web_server.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <spdlog/spdlog.h>
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
            spdlog::critical("Cannot run in the background");
            return 1;
        }
    }

    try
    {        
        spdlog::info( "Starting MailDWeb server...");
        web_server server(options);
        server.run();
        spdlog::info( "Stopping MailDWeb server...");
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        spdlog::error("Error occurred, shutting down. Cause: {}",ex.what());
        return 1;
    }
    catch(...)
    {
      spdlog::error( "Error occurred, shutting down.");
      return 1;
    }
    
    return 0;
}
