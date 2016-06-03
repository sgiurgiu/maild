#ifndef MAIL_H
#define MAIL_H
#include <string>

namespace maild {

    struct mail {
        std::string from;
        std::string to;
        std::string body;
    };
}
#endif // MAIL_H
