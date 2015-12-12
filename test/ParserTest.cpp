#include <gtest/gtest.h>

#include <sql/SQLParser.h>
#include <sql/SQLStatement.h>
#include <memory>
#include <sql/SQLParserException.h>

namespace parser_tests
{
    TEST(ParserCommonTests, AllTypesSupportTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table mytable(field1 int, field2 double, field3 varchar(2));");
        ASSERT_NE(dynamic_cast<CreateTableStatement*>(statement.get()), nullptr);
    }

    TEST(ParserCommonTests, SpecialTableNameTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table my_table(field1 int);");
        ASSERT_NE(dynamic_cast<CreateTableStatement*>(statement.get()), nullptr);
    }

    TEST(ParserCommonTests, CaseIgnoreTest) {
        auto statement = SQLParser::Instance().ParseStatement("CReaTe tAble mYtable(field1 int, field2 double);");
        ASSERT_NE(dynamic_cast<CreateTableStatement*>(statement.get()), nullptr);
    }

    TEST(ParserCommonTests, SpacesIgnoreTest) {
        auto statement = SQLParser::Instance().ParseStatement("    create  table    mytable    (   field1    int,    field2    double,    field3    varchar(   2  )  )  ;   ");
        ASSERT_NE(dynamic_cast<CreateTableStatement*>(statement.get()), nullptr);
    }

    TEST(ParserCommonTests, WrongTableNameTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table 22mytable(field1 int);"), SQLParserException);
    }

    TEST(ParserCommonTests, WrongTypeNameTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(field2 float);"), SQLParserException);
    }
    
    /* Remove if mot a bug */
    /*
    TEST(ParserCommonTests, WrongCaseTypeNameTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(field1 Int);"), SQLParserException);
    }
    */

    TEST(ParserCommonTests, WrongFieldNameTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(33field int);"), SQLParserException);
    }

    TEST(ParserCommonTests, WrongStatementNameTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create rable mytable(field1 int);"), SQLParserException);
    }

    TEST(ParserCommonTests, WrongRedurantTypeTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(field1 int int);"), SQLParserException);
    }
}


