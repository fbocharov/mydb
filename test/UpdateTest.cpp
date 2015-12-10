#include <sql/SQLStatement.h>
#include <gtest/gtest.h>
#include <sql/SQLParser.h>
#include <sql/SQLParserException.h>

namespace parser_tests
{
    /* update statement. Part 2 */
    /*
    TEST(UpdateTests, BasicTest) {
        auto statement = SQLParser::Instance().ParseStatement("update table set col1 = 23 where col2 = 12;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(UpdateTests, MultipleFieldsForUpdateTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("update table set col1 = 23, col2 = 12  where col2 = 12;"), SQLParserException);
    }

    TEST(UpdateTests, WrongDontForgetfromConditionTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("update table set col1 = 23, col2 = 12 ;"), SQLParserException);
    }
    */
}
