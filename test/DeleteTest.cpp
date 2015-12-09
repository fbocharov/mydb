#include <sql/SQLStatement.h>
#include <gtest/gtest.h>
#include <sql/SQLParser.h>
#include <sql/SQLParserException.h>

namespace parser_tests
{
    /* delete statement. Part 2
    TEST(DeleteTests, BasicTest) {
    auto statement = SQLParser::Instance().ParseStatement("delete from table1 where col = val;");
    ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(DeleteTests, WrongDontForgetWhereConditionTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("delete from table1;"), SQLParserException);
    }

    TEST(DeleteTests, WrongDontForgetfromConditionTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("delete where col = val;"), SQLParserException);
    }
    */
}
