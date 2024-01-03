#ifndef MAILD_WEB_NOT_FOUND_EXCEPTION_H
#define MAILD_WEB_NOT_FOUND_EXCEPTION_H

#include <stdexcept>
	

namespace maild {

class web_not_found_exception : public std::runtime_error
{
public:
    web_not_found_exception(const std::string& what):std::runtime_error(what){}
    web_not_found_exception(const char* what): std::runtime_error(what) {}
    
};
}

#endif // MAILD_WEB_NOT_FOUND_EXCEPTION_H
