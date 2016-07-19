#include "web_api_server.h"
#include <pqxx/transaction>

#define PICOJSON_USE_INT64
#include "picojson.h"
#include "crow.h"
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace maild;

web_api_server::web_api_server(const std::string& db_conn_string):db(db_conn_string)
{
    db.prepare("get_users_mails","select from_address,to_address,body,date_received,id from mails where username=$1 order by date_received desc");
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
        mail_row["from"] = picojson::value(row[0].c_str());
        mail_row["to"] = picojson::value(row[1].c_str());
        mail_row["body"] = picojson::value(row[2].c_str());
        mail_row["date"] = picojson::value(row[3].c_str());
        mail_row["id"] = picojson::value(row[4].as<int64_t>());
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
