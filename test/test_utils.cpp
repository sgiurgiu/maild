#include "gtest/gtest.h"
#include <utils.h>

using namespace maild;
using namespace std;

TEST(UtilsTest, ParseAddress)
{
  EXPECT_EQ(utils::parse_address("<a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::parse_address("<>"),"");
  EXPECT_EQ(utils::parse_address("aaa>"),"");
  EXPECT_EQ(utils::parse_address("aaa"),"");
  EXPECT_EQ(utils::parse_address("<aaa"),"");
}

TEST(UtilsTest,GetMailFrom)
{
  EXPECT_EQ(utils::get_mail_from(" FROM:<a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_from(" FROM: <a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_from(" FROM:a@a.com>"),"");
  EXPECT_EQ(utils::get_mail_from(" :<a@a.com>"),"");
}

TEST(UtilsTest,GetMailTo)
{
  EXPECT_EQ(utils::get_mail_to(" TO:<a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_to(" TO: <a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_to(" TO:a@a.com>"),"");
  EXPECT_EQ(utils::get_mail_to(" :<a@a.com>"),"");
}
TEST(UtilsTest,ParseBodyHtmlPart)
{
    ifstream test_file("data/test.eml");
    string html_body = utils::get_part(test_file,{"html"});
    cout << html_body<<endl;
    EXPECT_EQ(html_body.length(),271u);
}

TEST(UtilsTest,ParseBodyPlainPart)
{
    ifstream test_file("data/test.eml");
    std::string html_body = utils::get_part(test_file,{"plain"});
    EXPECT_EQ(html_body.length(),39u);
}
