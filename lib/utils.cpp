#include "utils.h"
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <glib.h>
#include <gmime/gmime.h>

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

/*std::string::size_type utils::get_next_utf8_part(const std::string& subj,std::string& decodedString)
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
}*/
/*std::string utils::parse_utf8_string(const std::string& subj)
{
    std::string decodedString;
    std::string subjCopy = subj;
    std::string::size_type pos;
    while((pos = get_next_utf8_part(subjCopy,decodedString)) != subj.npos)
    {
        subjCopy = subjCopy.substr(pos);
    }

    return decodedString;
}*/
std::optional<std::string> utils::get_subject(std::istream &in)
{
    GMimeStream* stream;
    GMimeParser *parser;
    GMimeMessage *message;
    std::string s(std::istreambuf_iterator<char>(in), {});    
    stream = g_mime_stream_mem_new_with_buffer(s.data(), s.length());
    parser = g_mime_parser_new_with_stream(stream);
    g_object_unref(stream);
    auto options = g_mime_parser_options_new();
    g_mime_parser_options_set_parameter_compliance_mode(options, GMIME_RFC_COMPLIANCE_LOOSE);

    message = g_mime_parser_construct_message(parser, options);    
    auto subject = g_mime_utils_header_decode_text(options, g_mime_message_get_subject(message));
    g_mime_parser_options_free(options);
    g_object_unref(parser);
    g_object_unref(message);

    if(!subject) return {};
    spdlog::debug("subject:{}",subject);
    
    return std::make_optional<std::string>(subject);
}

namespace
{
    struct get_message_part
    {
        //the types we're looking for
        //returning the body of the first part we're finding
        std::vector<std::string> types;
        //the body that we found
        std::string body;
    };

    void gmime_get_part(GMimeObject *, GMimeObject *part, gpointer user_data)
    {
        get_message_part *message_part = static_cast<get_message_part *>(user_data);
        if (GMIME_IS_MESSAGE_PART(part))
        {
            /* message/rfc822 or message/news */
            GMimeMessage *message;

            /* g_mime_message_foreach() won't descend into
                       child message parts, so if we want to count any
                       subparts of this child message, we'll have to call
                       g_mime_message_foreach() again here. */

            message = g_mime_message_part_get_message((GMimeMessagePart *)part);
            g_mime_message_foreach(message, gmime_get_part, message_part);
        }
        /*else if (GMIME_IS_MULTIPART(part))
        {
            GMimeMultipart *mpart = GMIME_MULTIPART(part);
            int count = g_mime_multipart_get_count(mpart);
            for (int i = 0; i < count; i++)
            {
                GMimeObject *subpart = g_mime_multipart_get_part(mpart, i);
                auto contentType = g_mime_object_get_content_type(subpart);
                auto subType = g_mime_content_type_get_media_subtype(contentType);
                for (const auto &type : message_part->types)
                {
                    if (type == subType)
                    {
                        auto content = g_mime_part_get_content((GMimePart *)subpart);
                        auto output_stream = g_mime_stream_mem_new();
                        auto message_length = g_mime_data_wrapper_write_to_stream(content, output_stream);
                        std::string body;
                        body.resize(message_length+1);
                        auto message_length_read = g_mime_stream_read(output_stream, body.data(), message_length);
                        g_object_unref(output_stream);
                        if(message_length_read < 0)
                        {
                            body = "N/A";
                        }
                        
                        message_part->body = body;
                        return;
                    }
                }
                return;
            }
        }*/
        else if (GMIME_IS_PART(part))
        {
            /* a normal leaf part, could be text/plain or
                * image/jpeg etc */
            auto contentType = g_mime_object_get_content_type(part);
            //auto subType = g_mime_content_type_get_media_subtype(contentType);
            for (const auto &type : message_part->types)
            {
                if (g_mime_content_type_is_type(contentType, "text", type.c_str()))
                {
                    auto content = g_mime_part_get_content((GMimePart *)part);
                    auto output_stream = g_mime_stream_mem_new();
                    auto message_length = g_mime_data_wrapper_write_to_stream(content, output_stream);
                    g_mime_stream_flush(output_stream);
                    auto array = g_mime_stream_mem_get_byte_array((GMimeStreamMem*)output_stream);
                    std::string body((char*)array->data, array->len);
                    g_object_unref(output_stream);
                    message_part->body = body;
                    return;
                }
            }
        }
    }

} // anonymous namespace

std::string utils::get_part(std::istream &in, const std::vector<std::string> &types)
{
    GMimeStream *stream;
    GMimeParser *parser;
    GMimeMessage *message;
    std::string s(std::istreambuf_iterator<char>(in), {});
    stream = g_mime_stream_mem_new_with_buffer(s.data(), s.length());
    parser = g_mime_parser_new_with_stream(stream);
    g_object_unref(stream);
    message = g_mime_parser_construct_message(parser, NULL);
    g_object_unref(parser);

    get_message_part message_part;
    message_part.types = types;
    g_mime_message_foreach(message, gmime_get_part, &message_part);

    g_object_unref(message);
    return message_part.body;
}
/*
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
*/