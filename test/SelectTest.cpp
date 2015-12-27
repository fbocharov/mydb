#include <gtest/gtest.h>

#include <sql/SQLParser.h>
#include <sql/SQLStatement.h>
#include <memory>
#include <sql/SQLParserException.h>

namespace parser_tests
{
    TEST(SelectTests, BasicTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from my_table;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, BasicStatementTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from my_table;");
        auto select_statement = dynamic_cast<SelectStatement*>(statement.get());
        ASSERT_EQ(select_statement->GetTableName(), "my_table");
    }

    /* select statement with where tests. Part 2 */
	TEST(SelectTests, WhereIntConditionTest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1=1;");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}

	TEST(SelectTests, WhereVarcharConditionTest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1=\"It is varchar string\";");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}

	TEST(SelectTests, WhereDoubleConditionTest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1=1.0;");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}

	TEST(SelectTests, WhereSpacedConditionTest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1 = 1;");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}
    /* Where statement with where multicondition tests. Part 3. or advanced part 2 */
    TEST(SelectTests, WhereStatementTwoAndConditionsTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1=1 and f2=\"string too  \";");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, WhereStatementThreeAndConditionsTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1=1 and f2=\"string too\" and f3=2.5;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, WhereStatementFourAndConditionsTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from my_table where f1=1 and f2=\"string too\" and f3=2.5 and f4 = 23.6;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }
	/* Not wildcard select query */
    /*
    TEST(SelectTests, OneFieldTest) {
        auto statement = SQLParser::Instance().ParseStatement("select f1 from my_table;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, TwoFieldsTest) {
        auto statement = SQLParser::Instance().ParseStatement("select f1, f2 from my_table;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, ThreeFieldsTest) {
        auto statement = SQLParser::Instance().ParseStatement("select f1,f2,f3 from my_table;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, SpacedThreeFieldsTest) {
        auto statement = SQLParser::Instance().ParseStatement("select f1 , f2   , f3 from my_table;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, CombineWithWhereConditionTest) {
        auto statement = SQLParser::Instance().ParseStatement("select f1, f2, f3 from my_table where f1=4;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }
    */
    /* Where statement and join tests. Part 3 */
    
    /* Compare where statements */
	/*
	TEST(SelectTests, LTTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from table1 where table1.col1 < 54;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

	TEST(SelectTests, LETest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from table1 where table1.col1 <= 54;");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}

    TEST(SelectTests, GTTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from table1 where table1.col1 > 54;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

	TEST(SelectTests, GETest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from table1 where table1.col1 >= 54;");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}

	TEST(SelectTests, NEQTest) {
		auto statement = SQLParser::Instance().ParseStatement("select * from table1 where table1.col1 != 54;");
		ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
	}
	*/
	/* Join only statments */
    /*
    TEST(SelectTests, SimpleJoinTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from table1 join table1 on table1.col1=table2.col2;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, SpacedSimpleJoinTest) {
        auto statement = SQLParser::Instance().ParseStatement("select * from table1 join table1 on table1.col1 = table2.col2;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, CombineWithPartSelectJoinTest) {
        auto statement = SQLParser::Instance().ParseStatement("select col3, table1.col1, table2.col2 from table1 join table1 on table1.col1 = table2.col2;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }
    */
    /* Hardest select queries */
    /*
    TEST(SelectTests, FirstPartJoinMultiWhereTest) {
        auto statement = SQLParser::Instance().ParseStatement("select table1.col2, table2.col2 from table1 join table2 on table1.col1 = table2.col2 where table1.col1 = \"Are you want use a haskell?\" and table2.col2 < 5;");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }

    TEST(SelectTests, SecondPartJoinMultiWhereTest) {
        auto statement = SQLParser::Instance().ParseStatement("select col1, t2.col2 from t1 join t2 on t1.col1 = t2.field2 where t1.col2 >= 4 and t2.field3 = \"Are you fell a pain?\" ");
        ASSERT_NE(dynamic_cast<SelectStatement*>(statement.get()), nullptr);
    }
    */
}
