#ifndef MAILD_UTILS_H
#define MAILD_UTILS_H
#include <string>
#include <vector>
#include <istream>
#include <mimetic/mimetic.h>
#include "toml.hpp"

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
        static std::string parse_utf8_string(const std::string& subj);

        static std::string get_subject(std::istream& in)
        {
            mimetic::MimeEntity me(in);
            return parse_utf8_string(me.header().subject());
        }
        
        static std::string get_part(std::istream& in,const std::vector<std::string>& types)
        {
            mimetic::MimeEntity me(in);                   
            return get_part(&me,types);            
        }
        static void configure_logs(const toml::node_view<toml::node>& node);
    private:
        static std::string::size_type get_next_utf8_part(const std::string& subj, std::string& decodedString);
        static std::string get_part(mimetic::MimeEntity* me,const std::vector<std::string>& types, std::string boundary = "");
        

    };
}
#endif // UTILS_H
