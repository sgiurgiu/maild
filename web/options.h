#ifndef MAILD_OPTIONS_H
#define MAILD_OPTIONS_H

#include <vector>
#include <string>
#include <ostream>

namespace maild {
struct options
{
    std::string config_file;
    std::string log_file;
    bool help=false;
    bool quiet=false;
    bool verbose=false;
    bool version=false;
    bool daemon = false;
};


}

#endif
