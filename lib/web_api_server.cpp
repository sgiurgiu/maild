#include "web_api_server.h"
#include <pqxx/transaction>

#define PICOJSON_USE_INT64
#include "picojson.h"
#include "crow_all.h"
#include "utils.h"
#include <chrono>
#include <iomanip>
#include <sstream>


using namespace maild;
log4cxx::LoggerPtr web_api_server::logger(log4cxx::Logger::getLogger("web_api_server"));

web_api_server::web_api_server()
{
}
web_api_server::web_api_server(const std::string& db_conn_string):db(db_conn_string)
{
    db.prepare("get_users_mails","select from_address,to_address,body,date_received,id from mails where username=$1 order by date_received desc");
    db.prepare("get_mail","select body from mails where id=$1");
}

crow::response web_api_server::get_users_mails(const std::string& user)
{    
    std::lock_guard<std::mutex> lock(mu);//this is awfullllllll. TODO: get rid of this mutex
    
    pqxx::work w(db);
    pqxx::result result = w.prepared("get_users_mails")
      (user)
      .exec();      
    w.commit();        
    const int num_rows = result.size();
    picojson::array mails_array;    
    for (int rownum=0; rownum < num_rows; ++rownum)
    {
        const pqxx::tuple row = result[rownum];
        picojson::object mail_row;
        std::string body_raw = row[2].as<std::string>();
        mail_row["from"] = picojson::value(row[0].c_str());
        mail_row["to"] = picojson::value(row[1].c_str());        
        mail_row["date"] = picojson::value(row[3].c_str());
        mail_row["id"] = picojson::value(row[4].as<int64_t>());
        std::stringstream body_raw_stream(body_raw);        
        mail_row["subject"] = picojson::value(utils::get_subject(body_raw_stream));
        mails_array.push_back(picojson::value(mail_row));
    }    
    picojson::value val(mails_array);
    std::string contents = val.serialize(false);
    crow::response rsp;
    rsp.code = 200;
    rsp.set_header("Content-Length",std::to_string(contents.length()));        
    rsp.set_header("Content-Type","application/json; charset=UTF-8");    
    rsp.write(contents);
    return rsp;
}
crow::response web_api_server::get_mail(int id, const std::string& type)
{    
    pqxx::work w(db);
    pqxx::result result = w.prepared("get_mail")
      (id)
      .exec();      
    w.commit();    
    if(result.size() == 1)
    {
        crow::response rsp;
        rsp.code = 200;
        
        std::string body_raw = result[0][0].as<std::string>();
        if(type == "raw")
        {
            rsp.set_header("Content-Length",std::to_string(body_raw.length()));        
            rsp.set_header("Content-Type","text/plain; charset=UTF-8");    
            rsp.write(body_raw);            
        } 
        else if (type == "html")
        {
            std::stringstream body_raw_stream(body_raw);
            auto html = utils::get_part(body_raw_stream,{"html"});
            rsp.set_header("Content-Length",std::to_string(html.length()));        
            rsp.set_header("Content-Type","text/html; charset=UTF-8");    
            rsp.write(html);            
        } 
        else if (type == "text")
        {
            std::stringstream body_raw_stream(body_raw);
            auto text = utils::get_part(body_raw_stream,{"text","plain"});
            rsp.set_header("Content-Length",std::to_string(text.length()));        
            rsp.set_header("Content-Type","text/plain; charset=UTF-8");    
            rsp.write(text);            
        }        
        return rsp;
    }
    else
    {
        crow::response rsp;
        rsp.code = 404;
        rsp.write("Not found");
        return rsp;
    }
}


