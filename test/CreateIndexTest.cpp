#include <sql/SQLStatement.h>
#include <gtest/gtest.h>
#include <sql/SQLParser.h>
#include <sql/SQLParserException.h>

namespace parser_tests
{
    /* Create index statement. Part 2
    TEST(CreateIndexTests, BasicTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (colname) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, UniqueIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create unique index index_name on tablename (colname) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, OrderedAscIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (colname asc) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, OrderedDescIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (colname desc) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, TwoColumnsIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (col1, col2) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, ThreeColumnsIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (col1, col2, col3) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, FewOrderedColumnsIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (col1 desc, col2 asc) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, FewWithSingleOrderedColumnsIndexTest) {
        auto statement = SQLParser::Instance().ParseStatement("create index index_name on tablename (col1, col2 asc) using btree;");
        ASSERT_NE(dynamic_cast<CreateStatement*>(statement.get()), nullptr);
    }

    TEST(CreateIndexTests, WrongDontForgetBtreeTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create index index_name on tablename (colname);"), SQLParserException);
    }

    TEST(CreateIndexTests, WrongDontForgetOnKeywordTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create index index_name tablename (colname) using btree;"), SQLParserException);
    }

    TEST(CreateIndexTests, WrongOrderTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create index index_name tablename (colname acs) using btree;"), SQLParserException);
    }
    */
}
