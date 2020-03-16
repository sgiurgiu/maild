#include "utils.h"
#include <boost/algorithm/string.hpp>

using namespace maild;

std::string::size_type utils::get_next_utf8_part(const std::string& subj,std::string& decodedString)
{
    if(subj.empty())
    {
        return subj.npos;
    }
    auto lowersubj = boost::algorithm::to_lower_copy(subj);
    auto startUtf8Header = lowersubj.find("=?utf-8?");
    if(startUtf8Header > 0 || startUtf8Header == lowersubj.npos)
    {
        decodedString.append(subj.substr(0,startUtf8Header));
        return startUtf8Header;
    }
    if(lowersubj.find("?=") == lowersubj.npos)
    {
        decodedString.append(subj);
        return subj.npos;
    }
    if(subj.size() < 12)
    {
        decodedString.append(subj);
        return subj.npos;
    }
    char encoding = subj[8];
    if(encoding != 'Q' && encoding != 'q' && encoding != 'B' && encoding != 'b')
    {
        decodedString.append(subj);
        return subj.npos;
    }
    auto utf8StartMarker = 9;
    auto utf8EndMarker = subj.find("?=",utf8StartMarker+1);
    std::string stringToDecode = subj.substr(utf8StartMarker+1,utf8EndMarker-utf8StartMarker-1);
    if(encoding == 'Q' || encoding == 'q')
    {
        mimetic::QP::Decoder decoder;
        decoder.process(stringToDecode.begin(), stringToDecode.end(),
                        std::back_inserter<std::string>(decodedString));
    }

    if(encoding == 'B' || encoding == 'b')
    {
        mimetic::Base64::Decoder decoder;
        decoder.process(stringToDecode.begin(), stringToDecode.end(),
                        std::back_inserter<std::string>(decodedString));
    }
    return  utf8EndMarker+2;
}
std::string utils::parse_utf8_string(const std::string& subj)
{
    std::string decodedString;
    std::string subjCopy = subj;
    std::string::size_type pos;
    while((pos = get_next_utf8_part(subjCopy,decodedString)) != subj.npos)
    {
        subjCopy = subjCopy.substr(pos);
    }

    return decodedString;
}

std::string utils::get_part(mimetic::MimeEntity* me,
                                   const std::vector<std::string>& types,
                                   std::string boundary)
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
            //LOG4CXX_DEBUG(logger, "have body :"<<body<<" of length "<<body.length());
            if(boundary.length() > 0)
            {
                //LOG4CXX_DEBUG(logger, "have boundary :"<<boundary);
                auto pos = body.find("--"+boundary);
                if(pos != std::string::npos)
                {
                    //LOG4CXX_DEBUG(logger, "found boundary at pos :"<<pos);
                    body.erase(pos);
                }
                else
                {
                    pos = body.find(boundary);
                    if(pos != std::string::npos)
                    {
                        //LOG4CXX_DEBUG(logger, "found boundary at pos :"<<pos);
                        body.erase(pos);
                    }

                }
            }

            auto transferEncoding = me->header().contentTransferEncoding().str();
            if(transferEncoding == mimetic::ContentTransferEncoding::quoted_printable)
            {
                mimetic::QP::Decoder decoder;

                std::string outBody;
                decoder.process(body.begin(), body.end(),std::back_inserter<std::string>(outBody));
                body.set(outBody);
            }

            //LOG4CXX_DEBUG(logger, "returning body :"<<body<<" of length "<<body.length());
            return std::move(body);
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
