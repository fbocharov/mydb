#include <gtest/gtest.h>

#include <sql/SQLParser.h>
#include <sql/SQLStatement.h>
#include <memory>
#include <sql/SQLParserException.h>

namespace parser_tests
{
    TEST(CreateTableTests, IntFieldTypeTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table mytable(field1 int);");
        auto createStatement = dynamic_cast<CreateTableStatement*>(statement.get());
        ASSERT_NE(createStatement, nullptr);
        auto columns = createStatement->GetColumns();
        ASSERT_EQ(columns.size(), 1);
        auto field1Column = columns.front();
        ASSERT_TRUE(strcmp(field1Column.name, "field1") == 0);
        ASSERT_EQ(field1Column.type, ValueType::INT);
    }

    TEST(CreateTableTests, DoubleFieldTypeTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table mytable(double_field double, field2 double);");
        auto createStatement = dynamic_cast<CreateTableStatement*>(statement.get());
        ASSERT_NE(createStatement, nullptr);
        auto columns = createStatement->GetColumns();
        ASSERT_EQ(columns.size(), 2);
        auto field1Column = columns.front();
        ASSERT_TRUE(strcmp(field1Column.name, "double_field") == 0);
        ASSERT_EQ(field1Column.type, ValueType::DOUBLE);
        ASSERT_EQ(columns[1].type, ValueType::DOUBLE);
    }

    TEST(CreateTableTests, VarcharTypeTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table mytable(f1 varchar(1));");
        auto createStatement = dynamic_cast<CreateTableStatement*>(statement.get());
        ASSERT_NE(createStatement, nullptr);
        auto columns = createStatement->GetColumns();
        ASSERT_EQ(columns.size(), 1);
        ASSERT_EQ(columns[0].type, ValueType::VARCHAR);
    }

    TEST(CreateTableTests, ShortTypesLengthTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table mytable(double_field double, field2 int);");
        auto createStatement = dynamic_cast<CreateTableStatement*>(statement.get());
        ASSERT_NE(createStatement, nullptr);
        auto columns = createStatement->GetColumns();
		ASSERT_EQ(columns[0].size, 8);
		ASSERT_EQ(columns[1].size, 4);
    }

    TEST(CreateTableTests, VarcharLengthTest) {
        auto statement = SQLParser::Instance().ParseStatement("create table mytable(f1 varchar(1), f2 varchar(30));");
        auto createStatement = dynamic_cast<CreateTableStatement*>(statement.get());
        ASSERT_NE(createStatement, nullptr);
        auto columns = createStatement->GetColumns();
        ASSERT_EQ(columns[0].size, 1);
        ASSERT_EQ(columns[1].size, 30);
    }

    TEST(CreateTableTests, WrongIntArrayTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(f1 int(2));"), SQLParserException);
    }

    TEST(CreateTableTests, WrongDoubleArrayTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(f1 double(2));"), SQLParserException);
    }

    TEST(CreateTableTests, WrongVarcharSingleTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(f1 varchar);"), SQLParserException);
    }

    TEST(CreateTableTests, WrongVarcharLengthTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(f1 varchar(-1));"), SQLParserException);
    }

    TEST(CreateTableTests, WrongVarcharNotDigitLengthTest) {
        ASSERT_THROW(SQLParser::Instance().ParseStatement("create table mytable(f1 varchar(a));"), SQLParserException);
    }
}
