%{
#include <cstring>
#include <cstddef>

#include "../common/Common.h"

#include "SQLStatement.h"
#include "SQLParserException.h"

#include "SQLParserInternals.hpp"

void yyerror(const char *);
int yylex(void);

%}

%option ansi-prototypes noyywrap yy_scan_string

%%

"select"          return SELECT;
"from"            return FROM;
"insert"          return INSERT;
"into"            return INTO;
"values"          return VALUES;
"update"          return UPDATE;
"set"             return SET;
"delete"          return DELETE;
"where"           return WHERE;
"and"             return AND;
"create"          return CREATE;
"table"           return TABLE;
"index"           return INDEX;
"unique"          return UNIQUE;
"on"              return ON;
"join"            return JOIN;
"using"           return USING;
"btree"           return BTREE;
"="               { return EQ; }
"<"               { yylval.string_v = strdup(yytext); return OP; }
">"               { yylval.string_v = strdup(yytext); return OP; }
"("               return *yytext;
")"               return *yytext;
","               return *yytext;
"*"               return *yytext;
";"               return *yytext;
"int"             return INT;
"double"          return DOUBLE;
"varchar"         return VARCHAR;
"exit"            return EXIT;
"quit"            return EXIT;
[0-9]+            { yylval.string_v = strdup(yytext); return INT_NUM;    }
[0-9]+\.[0-9]+    { yylval.string_v = strdup(yytext); return DOUBLE_NUM; }
\"[ \ta-z0-9[:punct:]]*\"  { yylval.string_v = strdup(yytext); return WORD;       }
[a-z]+[a-z0-9_]*  { yylval.string_v = strdup(yytext); return IDENTIFIER; }

[a-z]+[a-z0-9_]*\.[a-z]+[a-z0-9_]*  { yylval.string_v = strdup(yytext); return FULL_QUAL_NAME; }

[ \t]             {}
\r                {}
.                 { yyerror("Unknown Character"); return UNKNOWN_TOKEN; }

%%
