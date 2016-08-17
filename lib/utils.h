#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>
#include <istream>
#include <mimetic/mimetic.h>
#include <log4cxx/logger.h>

namespace maild {
    class utils final{
    private:
        utils() = delete;
        ~utils() = delete;
        utils ( const utils& ) = delete;
        utils ( const utils&& ) = delete;
        utils& operator= ( const utils& ) = delete;
        utils& operator= ( const utils&& ) = delete;
    public:
        static std::string get_mail_from(const std::string& line)
        {
            return get_mail_from_command(line,"FROM:");
        }
        static std::string get_mail_to(const std::string& line)
        {
            return get_mail_from_command(line,"TO:");
        }

        static std::string get_mail_from_command(const std::string& line, const std::string& prefix)
        {
            size_t index = line.find(prefix);
            if(index == std::string::npos) return "";

            std::string recipient = line.substr(index+prefix.size());
            return parse_address(recipient);
        }

        static std::string parse_address(const std::string& address)
        {
            size_t index1 = address.find('<');
            size_t index2 = address.find_last_of('>');
            if(index1 == std::string::npos || index2 == std::string::npos) return "";

            return address.substr(index1+1,index2-index1-1);
        }
        static std::string get_subject(std::istream& in)
        {
            mimetic::MimeEntity me(in);
            return me.header().subject();
        }
        
        static std::string get_part(std::istream& in,const std::vector<std::string>& types)
        {
            mimetic::MimeEntity me(in);                   
            return get_part(&me,types);            
        }
    private:
        static std::string get_part(mimetic::MimeEntity* me,const std::vector<std::string>& types, std::string boundary = "")
        {
            log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("utils"));
            std::string subtype = me->header().contentType().subtype();
            if(me->header().contentType().isMultipart())
            {
               boundary =  me->header().contentType().param("boundary");               
            }
            for(const auto& type : types)
            {
                if(subtype == type) 
                {
                    auto body = me->body();   
                    LOG4CXX_DEBUG(logger, "have body :"<<body<<" of length "<<body.length());
                    if(boundary.length() > 0)
                    {
                        LOG4CXX_DEBUG(logger, "have boundary :"<<boundary);
                        auto pos = body.find("--"+boundary);
                        if(pos != std::string::npos)
                        {
                            LOG4CXX_DEBUG(logger, "found boundary at pos :"<<pos);
                            body.erase(pos);
                        } 
                        else 
                        {
                            pos = body.find(boundary);
                            if(pos != std::string::npos)
                            {
                                LOG4CXX_DEBUG(logger, "found boundary at pos :"<<pos);
                                body.erase(pos);
                            } 
                            
                        }
                        
                    }
                    LOG4CXX_DEBUG(logger, "returning body :"<<body<<" of length "<<body.length());
                    return body;            
                }
            }    
            
            mimetic::MimeEntityList& parts = me->body().parts();
            mimetic::MimeEntityList::iterator mbit = parts.begin(), meit = parts.end();
            for(; mbit != meit; ++mbit)
            {
                auto part = get_part(*mbit,types,boundary);
                if(!part.empty()) return part;
            }
            return "";
        }
        

    };
}
#endif // UTILS_H
