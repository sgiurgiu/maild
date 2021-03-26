#include "utils.h"
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

using namespace maild;

void utils::configure_logs(const toml::node_view<toml::node>& logs_conf)
{
    std::vector<spdlog::sink_ptr> sinks;
    for(const auto& log_conf : *logs_conf.as_table())
    {
        auto& value = *(log_conf.second.as_table());
        std::string type = value["type"].value_or("console");
        auto level = value["level"].value_or("info");
        auto pattern = value["pattern"].value_or("[%D %T] [%l] %v");
        if(type == "console")
        {
            auto console = std::make_shared<spdlog::sinks::stdout_sink_mt>();
            console->set_pattern(std::string(pattern));
            console->set_level(spdlog::level::from_str(std::string(level)));
            sinks.push_back(console);
        }
        else if(type == "rotating_file")
        {
            auto file_name = value["file"].value_or("maild.log");
            auto max_size = value["max_size"].value_or(1000000);
            auto max_files = value["max_files"].value_or(10);
            auto file = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(std::string(file_name),max_size,max_files);
            file->set_pattern(std::string(pattern));
            file->set_level(spdlog::level::from_str(std::string(level)));
            sinks.push_back(file);
        }       
    }
    std::shared_ptr<spdlog::logger> all_logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(),sinks.end());
    all_logger->set_level(spdlog::level::trace);
    spdlog::register_logger(all_logger);
    spdlog::set_default_logger(all_logger);
    for(const auto& sink:all_logger->sinks())
    {
        if(sink->level() <= spdlog::level::debug)
        {
            spdlog::flush_on(spdlog::level::debug);
        }
    }
}

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
    auto utf8StartMarker = 10;
    auto utf8EndMarker = subj.find("?=",utf8StartMarker);
    std::string stringToDecode = subj.substr(utf8StartMarker,utf8EndMarker-utf8StartMarker);
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
    auto contentType = me->header().contentType();
    //auto type = contentType.type();
    std::string subtype = me->header().contentType().subtype();
    if(me->header().contentType().isMultipart())
    {
       boundary =  me->header().contentType().param("boundary");
    }
    auto charset = me->header().contentType().param("charset");
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
