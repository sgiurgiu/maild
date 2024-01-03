#ifndef MAIL_H
#define MAIL_H
#include <string>
#include <vector>

namespace maild {

    struct mail {
        std::string from;
        std::vector<std::string> to;
        std::string body;
    };
}
#endif // MAIL_H
