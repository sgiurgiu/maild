#ifndef UTILS_H
#define UTILS_H
#include <string>

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

    };
}
#endif // UTILS_H
