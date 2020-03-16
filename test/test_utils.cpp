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
    ifstream test_file("data/travel_spam.eml");
    EXPECT_TRUE(test_file.good());
    string html_body = utils::get_part(test_file,{"html"});
    EXPECT_EQ(html_body.length(),53017u);
}
TEST_F(UtilsTest,ParseBodyTextPartLinkedinEmail)
{
    ifstream test_file("data/travel_spam.eml");
    EXPECT_TRUE(test_file.good());
    string html_body = utils::get_part(test_file,{"text","plain"});
    EXPECT_EQ(html_body.length(),2816u);
}
TEST_F(UtilsTest, ParseUtf8SubjectPlain)
{
    EXPECT_EQ(utils::parse_utf8_string("aa"),"aa");
}
TEST_F(UtilsTest, ParseUtf8SubjectUtfEmoticonsWhole)
{
    std::string subj {"=?utf-8?Q?=E2=9C=88=EF=B8=8F=C2=A0=5BTRAVEL=20INSURANCE=5D=20|=20Optimal=20Travel=E2=84=A2=20Insurance?="};
    auto decoded = utils::parse_utf8_string(subj);
    std::string expected{"\xE2\x9C\x88\xEF\xB8\x8F\xC2\xA0[TRAVEL INSURANCE] | Optimal Travel\xE2\x84\xA2 Insurance"};
    EXPECT_EQ(decoded,expected);
}

TEST_F(UtilsTest, ParseUtf8SubjectUtfFullBlownComposed)
{
    std::string subj {"=?UTF-8?Q?=F0=9D=97=96=F0=9D=97=BC=F0=9D=97=BA?="
                      "=?UTF-8?Q?=F0=9D=97=BD=F0=9D=97=B9=F0=9D=97=B2?="
                      "=?UTF-8?Q?=F0=9D=98=81=F0=9D=97=B2_=F0=9D=98=81?="
                      "=?UTF-8?Q?=F0=9D=97=B5=F0=9D=97=B2_=F0=9D=98=80?="
                      "=?UTF-8?Q?=F0=9D=97=B5=F0=9D=97=BC=F0=9D=97=BF?="
                      "=?UTF-8?Q?=F0=9D=98=81_=F0=9D=97=B3=F0=9D=97=BC?="
                      "=?UTF-8?Q?=F0=9D=97=BF=F0=9D=97=BA?="};
    auto decoded = utils::parse_utf8_string(subj);
    std::string expected{"\xF0\x9D\x97\x96\xF0\x9D\x97\xBC\xF0\x9D\x97\xBA\xF0\x9D"
                         "\x97\xBD\xF0\x9D\x97\xB9\xF0\x9D\x97\xB2\xF0\x9D\x98\x81"
                         "\xF0\x9D\x97\xB2_\xF0\x9D\x98\x81\xF0\x9D\x97\xB5\xF0\x9D"
                         "\x97\xB2_\xF0\x9D\x98\x80\xF0\x9D\x97\xB5\xF0\x9D\x97\xBC"
                         "\xF0\x9D\x97\xBF\xF0\x9D\x98\x81_\xF0\x9D\x97\xB3\xF0\x9D"
                         "\x97\xBC\xF0\x9D\x97\xBF\xF0\x9D\x97\xBA"};
    EXPECT_EQ(decoded,expected);
}

TEST_F(UtilsTest, ParseUtf8SubjectUtfFullBlownComposedWithExtraCharsInside)
{
    std::string subj {"=?UTF-8?Q?=F0=9D=97=96=F0=9D=97=BC=F0=9D=97=BA?= CCSS "
                      "=?UTF-8?Q?=F0=9D=97=BD=F0=9D=97=B9=F0=9D=97=B2?= AAC"
                      "=?UTF-8?Q?=F0=9D=98=81=F0=9D=97=B2_=F0=9D=98=81?="};
    auto decoded = utils::parse_utf8_string(subj);
    std::string expected{"\xF0\x9D\x97\x96\xF0\x9D\x97\xBC\xF0\x9D\x97\xBA CCSS "
                         "\xF0\x9D\x97\xBD\xF0\x9D\x97\xB9\xF0\x9D\x97\xB2 AAC"
                         "\xF0\x9D\x98\x81\xF0\x9D\x97\xB2_\xF0\x9D\x98\x81"
                         };
    EXPECT_EQ(decoded,expected);
}
