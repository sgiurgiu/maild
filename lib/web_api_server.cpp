#include "web_api_server.h"
#include <pqxx/transaction>

#define PICOJSON_USE_INT64
#include "picojson.h"
#include "crow.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mimetic/mimetic.h>

using namespace maild;
log4cxx::LoggerPtr web_api_server::logger(log4cxx::Logger::getLogger("web_api_server"));

web_api_server::web_api_server(const std::string& db_conn_string):db(db_conn_string)
{
    db.prepare("get_users_mails","select from_address,to_address,body,date_received,id from mails where username=$1 order by date_received desc");
    db.prepare("get_mail","select body from mails where id=$1");
}

crow::response web_api_server::get_users_mails(const std::string& user)
{
    crow::response rsp;
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
        std::ios::sync_with_stdio(false);
        mimetic::MimeEntity me(body_raw_stream);
        mail_row["subject"] = picojson::value(me.header().subject());
        mails_array.push_back(picojson::value(mail_row));
    }    
    picojson::value val(mails_array);
    std::string contents = val.serialize(false);
    rsp.code = 200;
    rsp.set_header("Content-Length",std::to_string(contents.length()));        
    rsp.set_header("Content-Type","application/json; charset=UTF-8");    
    rsp.write(contents);
    return rsp;
}
crow::response web_api_server::get_mail(int id)
{
    crow::response rsp;
    pqxx::work w(db);
    pqxx::result result = w.prepared("get_mail")
      (id)
      .exec();      
    w.commit();
    if(result.size() == 1)
    {
        std::string body_raw = result[0][0].as<std::string>();
        picojson::object mail_row;
        mail_row["body_raw"]= picojson::value(body_raw);
        std::ios::sync_with_stdio(false);
        std::stringstream body_raw_stream(body_raw);
        mimetic::MimeEntity me(body_raw_stream);
        mail_row["body_html"]= picojson::value(get_part(&me,{"html"}));
        mail_row["body_plain"]= picojson::value(get_part(&me,{"text","plain"}));
        
        picojson::value val(mail_row);
        std::string contents = val.serialize(false);
        rsp.code = 200;
        rsp.set_header("Content-Length",std::to_string(contents.length()));        
        rsp.set_header("Content-Type","application/json; charset=UTF-8");    
        rsp.write(contents);
    }
    else
    {
        rsp.code = 404;
        rsp.write("Not found");
    }
    return rsp;
}
std::string web_api_server::get_part(mimetic::MimeEntity* me, const std::vector<std::string>& types)
{
    std::string xx = me->header().contentType().str();
    std::string subtype = me->header().contentType().subtype();
    for(const auto& type : types)
    {
        if(subtype == type) 
        {
            const auto& body = me->body();
            
            std::stringstream s;
            s << body;
            return s.str();            
        }
    }    
    
    mimetic::MimeEntityList& parts = me->body().parts();
    mimetic::MimeEntityList::iterator mbit = parts.begin(), meit = parts.end();
    for(; mbit != meit; ++mbit)
    {
	return get_part(*mbit,types);
    }
    return "";
}

