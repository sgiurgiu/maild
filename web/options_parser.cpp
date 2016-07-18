#include "options_parser.h"

using namespace maild;

options_parser::options_parser()
{
    visibleDescription.add_options()
    ("help,h","Get help using MAILD Web")
    ("config,c",boost::program_options::value<std::string>()->default_value("maild_web.conf"),"Set the maild web configuration file location")
    ("log,l",boost::program_options::value<std::string>()->default_value("log.properties"),"Set the maild web logging file location")    
    ("quiet,q","be quiet")
    ("verbose,v","be verbose")
    ("daemon,D","go in the background")
    ("version","show MAILD Web version and exit")
    ;

    optionsGroup.add(visibleDescription);
}


options options_parser::parse_arguments(int argc,char** argv)
{
    options opt;
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc,argv).
                                  options(optionsGroup).positional(positionalOd).run(),vm);
    boost::program_options::notify(vm);

    opt.help=vm.count("help");
    opt.quiet=vm.count("quiet");
    opt.verbose=vm.count("verbose");
    opt.version=vm.count("version");
    opt.daemon=vm.count("daemon");
    
    if(vm.count("config")) {
        opt.config_file=vm["config"].as<std::string>();
    }
    
    if(vm.count("log")) {
        opt.log_file=vm["log"].as<std::string>();
    }

    
    return opt;
}

void options_parser::show_help(std::ostream& os)
{
  os<<"Usage: maild_web <options>\n";
  os<<"Options:\n";
  visibleDescription.print(os);
}

void options_parser::show_version(std::ostream& os)
{
  os<<"maild_web version "<<MAILD_VERSION<<"\n";
}

