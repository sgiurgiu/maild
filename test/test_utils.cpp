#include "gtest/gtest.h"
#include <utils.h>

#include <log4cxx/basicconfigurator.h>

using namespace maild;
using namespace std;

class UtilsTest : public ::testing::Test {
 protected:
  void SetUp() override
  {
      log4cxx::BasicConfigurator::configure();
      log4cxx::LoggerPtr logger(log4cxx::Logger::getRootLogger());
      logger->setLevel(log4cxx::Level::getDebug());
  }

  // void TearDown() override {}
};

TEST_F(UtilsTest, ParseAddress)
{
  EXPECT_EQ(utils::parse_address("<a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::parse_address("<>"),"");
  EXPECT_EQ(utils::parse_address("aaa>"),"");
  EXPECT_EQ(utils::parse_address("aaa"),"");
  EXPECT_EQ(utils::parse_address("<aaa"),"");
}

TEST_F(UtilsTest,GetMailFrom)
{
  EXPECT_EQ(utils::get_mail_from(" FROM:<a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_from(" FROM: <a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_from(" FROM:a@a.com>"),"");
  EXPECT_EQ(utils::get_mail_from(" :<a@a.com>"),"");
}

TEST_F(UtilsTest,GetMailTo)
{
  EXPECT_EQ(utils::get_mail_to(" TO:<a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_to(" TO: <a@a.com>"),"a@a.com");
  EXPECT_EQ(utils::get_mail_to(" TO:a@a.com>"),"");
  EXPECT_EQ(utils::get_mail_to(" :<a@a.com>"),"");
}
TEST_F(UtilsTest,ParseBodyHtmlPart)
{
    ifstream test_file("data/test.eml");
    EXPECT_TRUE(test_file.good());
    string html_body = utils::get_part(test_file,{"html"});
    cout << html_body<<endl;
    EXPECT_EQ(html_body.length(),271u);
}

TEST_F(UtilsTest,ParseBodyPlainPart)
{
    ifstream test_file("data/test.eml");
    EXPECT_TRUE(test_file.good());
    std::string html_body = utils::get_part(test_file,{"plain"});
    EXPECT_EQ(html_body.length(),39u);
}

TEST_F(UtilsTest,ParseBodyHtmlPartLinkedinEmail)
{
    ifstream test_file("data/linkedin.eml");
    EXPECT_TRUE(test_file.good());
    string html_body = utils::get_part(test_file,{"html"});
    EXPECT_EQ(html_body.length(),88251u);
}
TEST_F(UtilsTest,ParseBodyTextPartLinkedinEmail)
{
    ifstream test_file("data/linkedin.eml");
    EXPECT_TRUE(test_file.good());
    string html_body = utils::get_part(test_file,{"text","plain"});
    EXPECT_EQ(html_body.length(),7298);
}
TEST_F(UtilsTest, ParseUtf8Subject)
{
    EXPECT_EQ(utils::parse_utf8_string("aa"),"aa");

    EXPECT_EQ(utils::parse_utf8_string("=?utf-8?Q?=E2=9C=88=EF=B8=8F=C2=A0=5BTRAVEL=20INSURANCE=5D=20|=20Optimal=20Travel=E2=84=A2=20Insurance?="),
            "a@a.com");
}
