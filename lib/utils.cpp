#include "utils.h"
#include <boost/algorithm/string.hpp>

using namespace maild;


std::string utils::parse_utf8_string(const std::string& subj)
{
    auto lowersubj = boost::algorithm::to_lower_copy(subj);
    if(subj.empty() || lowersubj.find("=?utf-8?") != 0
            || lowersubj.find("?=") == lowersubj.npos)
    {
        return subj;
    }
    if(subj.size() < 12) return subj;
    char encoding = subj[8];
    if(encoding != 'Q' && encoding != 'q' && encoding != 'B' && encoding != 'b')
    {
        return subj;
    }
    std::string stringToDecode = subj.substr(10,subj.size()-12);
    if(encoding == 'Q' || encoding == 'q')
    {
        mimetic::QP::Decoder decoder;

        std::string outString;
        decoder.process(stringToDecode.begin(), stringToDecode.end(),std::back_inserter<std::string>(outString));
        return outString;
    }

    if(encoding == 'B' || encoding == 'b')
    {
        mimetic::Base64::Decoder decoder;
        std::string outString;
        decoder.process(stringToDecode.begin(), stringToDecode.end(),std::back_inserter<std::string>(outString));
        return outString;
    }

    return subj;
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

            auto transferEncoding = me->header().contentTransferEncoding().str();
            if(transferEncoding == mimetic::ContentTransferEncoding::quoted_printable)
            {
                mimetic::QP::Decoder decoder;

                std::string outBody;
                decoder.process(body.begin(), body.end(),std::back_inserter<std::string>(outBody));
                body.set(outBody);
            }

            LOG4CXX_DEBUG(logger, "returning body :"<<body<<" of length "<<body.length());
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
