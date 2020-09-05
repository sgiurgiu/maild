#include "web_api_server.h"
#include <pqxx/transaction>
#include "json.hpp"
#include "utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>

#include <spdlog/spdlog.h>

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

web_api_server::response web_api_server::get_users_mails(const request& request, const std::string& user)
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
        pqxx::binarystring body_raw(row[2]);
        mail_row["from"] = row[0].as<std::string>();
        mail_row["to"] = row[1].as<std::string>();
        mail_row["date"] = row[3].as<std::string>();
        mail_row["id"] = row[4].as<int64_t>();
        std::stringstream body_raw_stream(body_raw.str());
        mail_row["subject"] = utils::get_subject(body_raw_stream);
        mails_array.push_back(mail_row);
    }
    std::string contents = mails_array.dump();
    response rsp;
    rsp.result(boost::beast::http::status::ok);
    rsp.version(request.version());
    rsp.set(boost::beast::http::field::server, MAILD_STRING);
    rsp.set(boost::beast::http::field::content_length, std::to_string(contents.length()));
    rsp.set(boost::beast::http::field::content_type,"application/json; charset=UTF-8");
    rsp.body() = (contents);
    rsp.prepare_payload();
    rsp.keep_alive(request.keep_alive());
    return rsp;
}
web_api_server::response web_api_server::get_mail(const request& request,int id,const std::string& type)
{    
    pqxx::work w(db);
    response rsp;
    try{
        pqxx::result result = w.exec_prepared("get_mail",id);
        w.commit();
        if(result.size() == 1)
        {
            rsp.result(boost::beast::http::status::ok);
            rsp.version(request.version());
            rsp.set(boost::beast::http::field::server, MAILD_STRING);
            
            pqxx::binarystring body_raw (result[0][0]);
            if(type == "raw")
            {
                rsp.set(boost::beast::http::field::content_length, std::to_string(body_raw.size()));
                rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
                rsp.body() = (body_raw.str());
            } 
            else if (type == "html")
            {
                std::stringstream body_raw_stream(body_raw.str());
                auto html = utils::get_part(body_raw_stream,{"html"});
                rsp.set(boost::beast::http::field::content_length, std::to_string(html.length()));
                rsp.set(boost::beast::http::field::content_type,"text/html; charset=UTF-8");
                rsp.body() = (html);
            } 
            else if (type == "text")
            {
                std::stringstream body_raw_stream(body_raw.str());
                auto text = utils::get_part(body_raw_stream,{"text","plain"});
                rsp.set(boost::beast::http::field::content_length, std::to_string(text.length()));
                rsp.set(boost::beast::http::field::content_type,"text/plain; charset=UTF-8");
                rsp.body() = (text);
            }
        }
        else
        {
            rsp.result(boost::beast::http::status::not_found);
            rsp.version(request.version());
            rsp.set(boost::beast::http::field::server, MAILD_STRING);
            rsp.body() = ("Not found");
        }
    }
    catch (const std::exception& e)
    {
        w.abort();
        throw;
    }
    rsp.prepare_payload();
    rsp.keep_alive(request.keep_alive());
    return rsp;
}


