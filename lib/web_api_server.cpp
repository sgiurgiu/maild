#include "web_api_server.h"
#include <pqxx/transaction>
#include "json.hpp"
#include "utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>

#include <spdlog/spdlog.h>


namespace
{
    constexpr auto SERVER_NAME = MAILD_STRING;
};

using json = nlohmann::json;
using namespace maild;


web_api_server::web_api_server()
{
}
web_api_server::web_api_server(const std::string& db_conn_string):db(db_conn_string)
{
    db.prepare("get_users_mails","select from_address,to_address,body,date_received,id from mails where username=$1 order by date_received desc");
    db.prepare("get_mail","select body from mails where id=$1");
}

web_api_server::response web_api_server::get_users_mails(const std::string& user)
{    
    pqxx::work w(db);
    pqxx::result result = w.exec_prepared("get_users_mails",user);
    w.commit();        
    const int num_rows = result.size();
    json mails_array = json::array();
    for (int rownum=0; rownum < num_rows; ++rownum)
    {
        const pqxx::row row = result[rownum];
        json mail_row = json::object();        
        std::basic_string<std::byte> body_raw = row[2].as<std::basic_string<std::byte>>();
        mail_row["from"] = row[0].as<std::string>();
        mail_row["to"] = row[1].as<std::string>();
        mail_row["date"] = row[3].as<std::string>();
        mail_row["id"] = row[4].as<int64_t>();
        std::string body_raw_str(
                    static_cast<const char *>(static_cast<const void *>(body_raw.data())),
                    body_raw.size());
        std::stringstream body_raw_stream(body_raw_str);
        mail_row["subject"] = utils::get_subject(body_raw_stream);
        mails_array.push_back(mail_row);
    }
    std::string contents = mails_array.dump();
    response rsp;
    rsp.result(boost::beast::http::status::ok);
    rsp.set(boost::beast::http::field::server, SERVER_NAME);
    rsp.set(boost::beast::http::field::content_length, std::to_string(contents.length()));
    rsp.set(boost::beast::http::field::content_type,"application/json; charset=UTF-8");
    rsp.body() = (contents);
    rsp.prepare_payload();
    return rsp;
}
web_api_server::response web_api_server::get_mail(int id,const std::string& type)
{    
    pqxx::work w(db);
    response rsp;
    try{
        pqxx::result result = w.exec_prepared("get_mail",id);
        w.commit();
        if(result.size() == 1)
        {
            rsp.result(boost::beast::http::status::ok);
            rsp.set(boost::beast::http::field::server, SERVER_NAME);
            std::basic_string<std::byte> body_raw = result[0][0].as<std::basic_string<std::byte>>();
            std::string body_raw_str(
                        static_cast<const char *>(static_cast<const void *>(body_raw.data())),
                        body_raw.size());

            std::stringstream body_raw_stream(body_raw_str);
            if(type == "raw")
            {
                rsp.set(boost::beast::http::field::content_length, std::to_string(body_raw.size()));
                rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
                rsp.body() = (body_raw_str);
            } 
            else if (type == "html")
            {
                auto html = utils::get_part(body_raw_stream,{"html"});
                rsp.set(boost::beast::http::field::content_length, std::to_string(html.length()));
                rsp.set(boost::beast::http::field::content_type,"text/html; charset=UTF-8");
                rsp.body() = (html);
            } 
            else if (type == "text")
            {
                auto text = utils::get_part(body_raw_stream,{"text","plain"});
                rsp.set(boost::beast::http::field::content_length, std::to_string(text.length()));
                rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
                rsp.body() = (text);
            }
        }
        else
        {
            rsp.result(boost::beast::http::status::not_found);
            rsp.set(boost::beast::http::field::server, SERVER_NAME);
            rsp.body() = ("Not found");
        }
    }
    catch (const std::exception& e)
    {
        w.abort();
        throw;
    }
    rsp.prepare_payload();
    return rsp;
}


