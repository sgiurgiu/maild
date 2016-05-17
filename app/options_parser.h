#ifndef MAILD_OPTIONS_PARSER_H
#define MAILD_OPTIONS_PARSER_H

#include <boost/program_options.hpp>
#include "options.h"

namespace maild {

    class options_parser
    {
        public:
            options_parser();
            options parse_arguments(int argc,char** argv);
            void show_help(std::ostream& os);
            void show_version(std::ostream& os);

        private:
            boost::program_options::options_description visibleDescription;
            boost::program_options::options_description hiddenDescription;
            boost::program_options::options_description optionsGroup;
            boost::program_options::positional_options_description positionalOd;
    };

} // namespace maild

#endif // MAILD_OPTIONS_PARSER_H
